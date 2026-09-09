#define NVOC_SUBDEVICE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_subdevice_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4b01b3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for Subdevice
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__Subdevice(Subdevice*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_Subdevice(Subdevice*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_Subdevice(Subdevice*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_Subdevice(Subdevice*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_Subdevice(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Subdevice;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Subdevice;

// Down-thunk(s) to bridge Subdevice methods from ancestors (if any)
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
void __nvoc_down_thunk_Subdevice_resPreDestruct(struct RsResource *pResource);    // this
NV_STATUS __nvoc_down_thunk_Subdevice_gpuresInternalControlForward(struct GpuResource *pSubdevice, NvU32 command, void *pParams, NvU32 size);    // this

// Up-thunk(s) to bridge Subdevice methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceControl(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceMap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceUnmap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_subdeviceShareCallback(struct Subdevice *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize(struct Subdevice *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceGetMapAddrSpace(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NvHandle __nvoc_up_thunk_GpuResource_subdeviceGetInternalObjectHandle(struct Subdevice *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_subdeviceAccessCallback(struct Subdevice *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceGetMemInterMapParams(struct Subdevice *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceCheckMemInterUnmap(struct Subdevice *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceGetMemoryMappingDescriptor(struct Subdevice *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceControlSerialization_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_subdeviceControlSerialization_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceControl_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_subdeviceControl_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_subdeviceCanCopy(struct Subdevice *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceIsDuplicate(struct Subdevice *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceControlFilter(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_subdeviceIsPartialUnmapSupported(struct Subdevice *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceMapTo(struct Subdevice *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceUnmapFrom(struct Subdevice *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_subdeviceGetRefCount(struct Subdevice *pResource);    // this
void __nvoc_up_thunk_RsResource_subdeviceAddAdditionalDependants(struct RsClient *pClient, struct Subdevice *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_subdeviceGetNotificationListPtr(struct Subdevice *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_subdeviceGetNotificationShare(struct Subdevice *pNotifier);    // this
void __nvoc_up_thunk_Notifier_subdeviceSetNotificationShare(struct Subdevice *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_subdeviceUnregisterEvent(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_subdeviceGetOrAllocNotifShare(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// Class-specific details for Subdevice
const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice = 
{
    .classInfo.size =               sizeof(Subdevice),
    .classInfo.classId =            classId(Subdevice),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Subdevice",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_Subdevice,
    .pCastInfo =          &__nvoc_castinfo__Subdevice,
    .pExportInfo =        &__nvoc_export_info__Subdevice
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00060048u)
static NV_STATUS subdeviceCtrlCmdBiosGetInfoV2__EXPORT(void *pSubdevice, void *pBiosInfoParams) {
    return subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(pSubdevice, pBiosInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00060048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBiosGetNbsiV2__EXPORT(void *pSubdevice, void *pNbsiParams) {
    return subdeviceCtrlCmdBiosGetNbsiV2_IMPL(pSubdevice, pNbsiParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001000au)
static NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo__EXPORT(void *pSubdevice, void *pBiosGetSKUInfoParams) {
    return subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(pSubdevice, pBiosGetSKUInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001000au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdBiosGetPostTime__EXPORT(void *pSubdevice, void *pBiosPostTime) {
    return subdeviceCtrlCmdBiosGetPostTime_DISPATCH(pSubdevice, pBiosPostTime);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport__EXPORT(void *pSubdevice, void *pUEFIParams) {
    return subdeviceCtrlCmdBiosGetUefiSupport_IMPL(pSubdevice, pUEFIParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdClkPmumonClkDomainsGetSamples__EXPORT(void *pSubdevice, void *pGetSamplesParams) {
    return subdeviceCtrlCmdClkPmumonClkDomainsGetSamples_DISPATCH(pSubdevice, pGetSamplesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001050bu)
static NV_STATUS subdeviceCtrlCmdMcGetArchInfo__EXPORT(void *pSubdevice, void *pArchInfoParams) {
    return subdeviceCtrlCmdMcGetArchInfo_IMPL(pSubdevice, pArchInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001050bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdMcGetManufacturer__EXPORT(void *pSubdevice, void *pManufacturerParams) {
    return subdeviceCtrlCmdMcGetManufacturer_IMPL(pSubdevice, pManufacturerParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership__EXPORT(void *pSubdevice, void *pReplayableFaultOwnrshpParams) {
    return subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL(pSubdevice, pReplayableFaultOwnrshpParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdMcServiceInterrupts__EXPORT(void *pSubdevice, void *pServiceInterruptParams) {
    return subdeviceCtrlCmdMcServiceInterrupts_IMPL(pSubdevice, pServiceInterruptParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS subdeviceCtrlCmdMcGetEngineNotificationIntrVectors__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdMcGetIntrCategorySubtreeMap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMcGetIntrCategorySubtreeMap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS subdeviceCtrlCmdMcGetStaticIntrTable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMcGetStaticIntrTable_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDmaInvalidateTLB_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdDmaGetInfo__EXPORT(void *pSubdevice, void *pDmaInfoParams) {
    return subdeviceCtrlCmdDmaGetInfo_IMPL(pSubdevice, pDmaInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010518u)
static NV_STATUS subdeviceCtrlCmdBusGetPciInfo__EXPORT(void *pSubdevice, void *pPciInfoParams) {
    return subdeviceCtrlCmdBusGetPciInfo_IMPL(pSubdevice, pPciInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010518u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010518u)
static NV_STATUS subdeviceCtrlCmdBusGetPciInfoV2__EXPORT(void *pSubdevice, void *pPciInfoParams) {
    return subdeviceCtrlCmdBusGetPciInfoV2_IMPL(pSubdevice, pPciInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010518u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdBusGetInfoV2__EXPORT(void *pSubdevice, void *pBusInfoParams) {
    return subdeviceCtrlCmdBusGetInfoV2_IMPL(pSubdevice, pBusInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010518u)
static NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo__EXPORT(void *pSubdevice, void *pBarInfoParams) {
    return subdeviceCtrlCmdBusGetPciBarInfo_IMPL(pSubdevice, pBarInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010518u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed__EXPORT(void *pSubdevice, void *pBusInfoParams) {
    return subdeviceCtrlCmdBusSetPcieSpeed_IMPL(pSubdevice, pBusInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth__EXPORT(void *pSubdevice, void *pLinkWidthParams) {
    return subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL(pSubdevice, pLinkWidthParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetPexCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPexCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusControlPublicAspmBits_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusClearPexCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusClearPexCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPexUtilCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusClearPexUtilCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusFreezePexCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusFreezePexCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPexLaneCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusSetEomParameters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusSetEomParameters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetEomStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetEomStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040448u)
static NV_STATUS subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040448u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdBusGetC2CInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetC2CInfo_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetC2CErrorInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetC2CErrorInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetC2CPacketCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetC2CPacketCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdBusGetC2CLpwrStats__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdBusSetC2CLpwrStateVote__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdBusSetC2CIdleThreshold__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusSetC2CIdleThreshold_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00001000u)
static NV_STATUS subdeviceCtrlCmdBusSysmemAccess__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusSysmemAccess_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00001000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050040u)
static NV_STATUS subdeviceCtrlCmdBusSetP2pMapping__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050040u)
static NV_STATUS subdeviceCtrlCmdBusUnsetP2pMapping__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedBWMode__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetSupportedBWMode_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetBWMode__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetBWMode_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetBWMode__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkSetBWMode_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetBWModeAsync__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkSetBWModeAsync_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetLocalDeviceInfo__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetupNvleEncryptionKey__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkSetupNvleEncryptionKey_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetLinkTrainingTime__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetLinkTrainingTime_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGetNvlinkSupportedCounters__EXPORT(void *arg_this, void *pParams) {
    return subdeviceCtrlCmdGetNvlinkSupportedCounters_IMPL(arg_this, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGetNvlinkCountersV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetNvlinkCountersV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdClearNvlinkCountersV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdClearNvlinkCountersV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGetNvlinkCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetNvlinkCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdClearNvlinkCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdClearNvlinkCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetNvlinkStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdBusGetNvlinkErrInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBusGetNvlinkErrInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkInjectSWError__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkInjectSWError_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetLinkAccumulativeMetricData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetLinkAccumulativeMetricData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetLinkRecordMetricData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetLinkRecordMetricData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetDeviceRecordMetricData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetDeviceRecordMetricData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkConfigureL1Toggle__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkConfigureL1Toggle_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlNvlinkGetL1Toggle__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlNvlinkGetL1Toggle_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinGetLinkFomValues__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinGetLinkFomValues_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkEccErrors__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetupEom__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkSetupEom_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetPowerState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkReadTpCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetLpCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkClearLpCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkClearLpCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010041u)
static NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010041u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdNvlinkEnableLinks_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010250u)
static NV_STATUS subdeviceCtrlCmdNvlinkInbandSendData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkInbandSendData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010250u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkPostFaultUp__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkPostFaultUp_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkPostLazyErrorRecovery__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkEomControl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkEomControl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetL1Threshold__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkSetL1Threshold_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkDirectConnectCheck__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkDirectConnectCheck_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetL1Threshold__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetL1Threshold_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdIsNvlinkReducedConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdIsNvlinkReducedConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetPortEvents__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetPortEvents_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkIsGpuDegraded__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkIsGpuDegraded_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkSetNvleEnabledState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkSetNvleEnabledState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkCycleLink__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkCycleLink_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkUpdateNvleTopology__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkUpdateNvleTopology_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetUpdateNvleLids__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetUpdateNvleLids_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetUpdateNvleLidsV2__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetUpdateNvleLidsV2_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkPRMAccess__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkPRMAccess_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetPlatformInfo__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetPlatformInfo_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo__EXPORT(void *arg_this, void *arg2) {
    return subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL__EXPORT(void *arg_this, void *pParams) {
    return subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL_IMPL(arg_this, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL__EXPORT(void *arg_this, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL_IMPL(arg_this, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdNvlinkUpdateClid__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkUpdateClid_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)
static NV_STATUS subdeviceCtrlCmdNvlinkLockRemapTableAndMse__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkLockRemapTableAndMse_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetRemapTableInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetRemapTableInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetRemapTableInfoV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetRemapTableInfoV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)
static NV_STATUS subdeviceCtrlCmdNvlinkGetNvlePktCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdNvlinkGetNvlePktCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000054u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdI2cReadBuffer__EXPORT(void *pSubdevice, void *pI2cParams) {
    return subdeviceCtrlCmdI2cReadBuffer_IMPL(pSubdevice, pI2cParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdI2cWriteBuffer__EXPORT(void *pSubdevice, void *pI2cParams) {
    return subdeviceCtrlCmdI2cWriteBuffer_IMPL(pSubdevice, pI2cParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdI2cReadReg__EXPORT(void *pSubdevice, void *pI2cParams) {
    return subdeviceCtrlCmdI2cReadReg_IMPL(pSubdevice, pI2cParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdI2cWriteReg__EXPORT(void *pSubdevice, void *pI2cParams) {
    return subdeviceCtrlCmdI2cWriteReg_IMPL(pSubdevice, pI2cParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdThermalSystemExecuteV2__EXPORT(void *pSubdevice, void *pSystemExecuteParams) {
    return subdeviceCtrlCmdThermalSystemExecuteV2_IMPL(pSubdevice, pSystemExecuteParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050008u)
static NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000004au)
static NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl__EXPORT(void *pSubdevice, void *pControlParams) {
    return subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL(pSubdevice, pControlParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000004au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl__EXPORT(void *pSubdevice, void *pControlParams) {
    return subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL(pSubdevice, pControlParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)
static NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw__EXPORT(void *pSubdevice, void *pPerfmonParams) {
    return subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(pSubdevice, pPerfmonParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdPerfSetAuxPowerState__EXPORT(void *pSubdevice, void *pPowerStateParams) {
    return subdeviceCtrlCmdPerfSetAuxPowerState_KERNEL(pSubdevice, pPowerStateParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdPerfSetPowerstate__EXPORT(void *pSubdevice, void *pPowerInfoParams) {
    return subdeviceCtrlCmdPerfSetPowerstate_IMPL(pSubdevice, pPowerInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdPerfGetLevelInfo_V2__EXPORT(void *pSubdevice, void *pLevelInfoParams) {
    return subdeviceCtrlCmdPerfGetLevelInfo_V2_DISPATCH(pSubdevice, pLevelInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdPerfGetCurrentPstate__EXPORT(void *pSubdevice, void *pInfoParams) {
    return subdeviceCtrlCmdPerfGetCurrentPstate_DISPATCH(pSubdevice, pInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__EXPORT(void *pSubdevice, void *pSampleParams) {
    return subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_DISPATCH(pSubdevice, pSampleParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdPerfGetPowerstate__EXPORT(void *pSubdevice, void *powerInfoParams) {
    return subdeviceCtrlCmdPerfGetPowerstate_DISPATCH(pSubdevice, powerInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdPerfNotifyVideoevent__EXPORT(void *pSubdevice, void *pVideoEventParams) {
    return subdeviceCtrlCmdPerfNotifyVideoevent_DISPATCH(pSubdevice, pVideoEventParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdPerfGetTegraPerfmonSample__EXPORT(void *pSubdevice, void *pSampleParams) {
    return subdeviceCtrlCmdPerfGetTegraPerfmonSample_IMPL(pSubdevice, pSampleParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010318u)
static NV_STATUS subdeviceCtrlCmdKPerfBoost__EXPORT(void *pSubdevice, void *pBoostParams) {
    return subdeviceCtrlCmdKPerfBoost_IMPL(pSubdevice, pBoostParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010318u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo__EXPORT(void *pSubdevice, void *pGFBRIParams) {
    return subdeviceCtrlCmdFbGetFBRegionInfo_IMPL(pSubdevice, pGFBRIParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFbGetBar1Offset__EXPORT(void *pSubdevice, void *pFbMemParams) {
    return subdeviceCtrlCmdFbGetBar1Offset_IMPL(pSubdevice, pFbMemParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS subdeviceCtrlCmdFbIsKind__EXPORT(void *pSubdevice, void *pIsKindParams) {
    return subdeviceCtrlCmdFbIsKind_IMPL(pSubdevice, pIsKindParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS subdeviceCtrlCmdFbGetMemAlignment__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetMemAlignment_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetHeapReservationSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000509u)
static NV_STATUS subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000509u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_395e98(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFbStatsGet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbStatsGet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdGspFbStatsGet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGspFbStatsGet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000148u)
static NV_STATUS subdeviceCtrlCmdFbGetCarveoutRegionInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetCarveoutRegionInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000148u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000514u)
static NV_STATUS subdeviceCtrlCmdFbGetCpuCoherentRange__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetCpuCoherentRange_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000514u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000554u)
static NV_STATUS subdeviceCtrlCmdFbGetWprRegionInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetWprRegionInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000554u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdFbGetInfoV2__EXPORT(void *pSubdevice, void *pFbInfoParams) {
    return subdeviceCtrlCmdFbGetInfoV2_IMPL(pSubdevice, pFbInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed__EXPORT(void *pSubdevice, void *pGCLFParams) {
    return subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL(pSubdevice, pGCLFParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdFbFlushGpuCache__EXPORT(void *pSubdevice, void *pCacheFlushParams) {
    return subdeviceCtrlCmdFbFlushGpuCache_IMPL(pSubdevice, pCacheFlushParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040148u)
static NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo__EXPORT(void *pSubdevice, void *pGpuCacheParams) {
    return subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL(pSubdevice, pGpuCacheParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040148u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages__EXPORT(void *pSubdevice, void *pOsOfflinedParams) {
    return subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL(pSubdevice, pOsOfflinedParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages__EXPORT(void *pSubdevice, void *pBlackListParams) {
    return subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(pSubdevice, pBlackListParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdFbSetupVprRegion__EXPORT(void *pSubdevice, void *pCliReqParams) {
    return subdeviceCtrlCmdFbSetupVprRegion_IMPL(pSubdevice, pCliReqParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050158u)
static NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050158u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbPatchPbrForMining_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)
static NV_STATUS subdeviceCtrlCmdFbGetRemappedRows__EXPORT(void *pSubdevice, void *pRemappedRowsParams) {
    return subdeviceCtrlCmdFbGetRemappedRows_IMPL(pSubdevice, pRemappedRowsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdFbGetFsInfo__EXPORT(void *pSubdevice, void *pInfoParams) {
    return subdeviceCtrlCmdFbGetFsInfo_IMPL(pSubdevice, pInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)
static NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbUpdateNumaStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS subdeviceCtrlCmdFbGetNumaInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetNumaInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0005c0c0u)
static NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0005c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbSetZbcReferenced_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysCleanLtcProbeFilter__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdMemSysCleanLtcProbeFilter_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbCBCOp__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbCBCOp_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbSetRrd__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbSetRrd_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbSetReadLimit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbSetReadLimit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFbSetWriteLimit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbSetWriteLimit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100008u)
static NV_STATUS subdeviceCtrlCmdGmmuCommitTlbInvalidate__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGmmuCommitTlbInvalidate_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFbGetStaticBar1Info__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetStaticBar1Info_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdFbGetUgpuMemoryInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFbGetUgpuMemoryInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__EXPORT(void *pSubdevice, void *pConfig) {
    return subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(pSubdevice, pConfig);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040044u)
static NV_STATUS subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__EXPORT(void *pSubdevice, void *pConfig) {
    return subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(pSubdevice, pConfig);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport__EXPORT(void *pSubdevice, void *pConfig) {
    return subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_IMPL(pSubdevice, pConfig);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__EXPORT(void *pSubdevice, void *pConfig) {
    return subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(pSubdevice, pConfig);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)
static NV_STATUS subdeviceCtrlCmdFbGetRemappedBanks__EXPORT(void *pSubdevice, void *pRemappedBanksParams) {
    return subdeviceCtrlCmdFbGetRemappedBanks_IMPL(pSubdevice, pRemappedBanksParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdSetGpfifo__EXPORT(void *pSubdevice, void *pSetGpFifoParams) {
    return subdeviceCtrlCmdSetGpfifo_IMPL(pSubdevice, pSetGpFifoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetPhysicalChannelCount_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00030008u)
static NV_STATUS subdeviceCtrlCmdFifoGetInfo__EXPORT(void *pSubdevice, void *pFifoInfoParams) {
    return subdeviceCtrlCmdFifoGetInfo_IMPL(pSubdevice, pFifoInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00030008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS subdeviceCtrlCmdFifoDisableChannels__EXPORT(void *pSubdevice, void *pDisableChannelParams) {
    return subdeviceCtrlCmdFifoDisableChannels_IMPL(pSubdevice, pDisableChannelParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__EXPORT(void *pSubdevice, void *pDisableChannelParams) {
    return subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(pSubdevice, pDisableChannelParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000100u)
static NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__EXPORT(void *pSubdevice, void *pDisableChannelParams) {
    return subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(pSubdevice, pDisableChannelParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000100u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFifoRotateKeys__EXPORT(void *pSubdevice, void *pRotateKeyParams) {
    return subdeviceCtrlCmdFifoRotateKeys_DISPATCH(pSubdevice, pRotateKeyParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040040u)
static NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoDisableUsermodeChannels_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010004u)
static NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo__EXPORT(void *pSubdevice, void *pChannelMemParams) {
    return subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL(pSubdevice, pChannelMemParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation__EXPORT(void *pSubdevice, void *pUserdLocationParams) {
    return subdeviceCtrlCmdFifoGetUserdLocation_DISPATCH(pSubdevice, pUserdLocationParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFifoObjschedSwGetLog__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoObjschedSwGetLog_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFifoObjschedGetState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoObjschedGetState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFifoObjschedSetState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoObjschedSetState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdFifoObjschedGetCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo__EXPORT(void *pSubdevice, void *pGetChannelGrpUidParams) {
    return subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_IMPL(pSubdevice, pGetChannelGrpUidParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFifoQueryChannelUniqueId__EXPORT(void *pSubdevice, void *pQueryChannelUidParams) {
    return subdeviceCtrlCmdFifoQueryChannelUniqueId_IMPL(pSubdevice, pQueryChannelUidParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFifoConfigCtxswTimeout__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoConfigCtxswTimeout_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0005c040u)
static NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0005c040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000244u)
static NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoClearFaultedBit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000244u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000068u)
static NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy__EXPORT(void *pSubdevice, void *pSchedPolicyParams) {
    return subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL(pSubdevice, pSchedPolicyParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000068u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo__EXPORT(void *pSubdevice, void *pChannelInfo) {
    return subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(pSubdevice, pChannelInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000001d8u)
static NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels__EXPORT(void *pSubdevice, void *pNumChannelsParams) {
    return subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL(pSubdevice, pNumChannelsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000001d8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdFifoGetAllocatedChannels__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalFifoGetNumSecureChannels__EXPORT(void *pSubdevice, void *pNumSecureChannelsParams) {
    return subdeviceCtrlCmdInternalFifoGetNumSecureChannels_IMPL(pSubdevice, pNumSecureChannelsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling__EXPORT(void *pSubdevice, void *pToggleActiveChannelSchedulingParams) {
    return subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_IMPL(pSubdevice, pToggleActiveChannelSchedulingParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdKGrGetInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdKGrGetInfoV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetInfoV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010au)
static NV_STATUS subdeviceCtrlCmdKGrGetCapsV2__EXPORT(void *pSubdevice, void *pGrCapsParams) {
    return subdeviceCtrlCmdKGrGetCapsV2_IMPL(pSubdevice, pGrCapsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetCtxswModes_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswZcullMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00090348u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswZcullBind_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00090348u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo__EXPORT(void *pSubdevice, void *pZcullInfoParams) {
    return subdeviceCtrlCmdKGrGetZcullInfo_IMPL(pSubdevice, pZcullInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswPmBind_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswSetupBind__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswSetupBind_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrSetGpcTileMap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrPcSamplingMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010348u)
static NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010348u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000408u)
static NV_STATUS subdeviceCtrlCmdKGrGetROPInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetROPInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000408u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetCtxswStats_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)
static NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00008000u)
static NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00008000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00008000u)
static NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00008000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetPpcMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetPpcMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)
static NV_STATUS subdeviceCtrlCmdGrTestCtxswErrorLogs__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrTestCtxswErrorLogs_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifierV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000918u)
static NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000918u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000014u)
static NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000014u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdKGrGetGpcMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetGpcMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdKGrGetTpcMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetTpcMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetGpcTileMap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010018u)
static NV_STATUS subdeviceCtrlCmdKGrGetZcullMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetZcullMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c1c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c1c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)
static NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c0c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010040u)
static NV_STATUS subdeviceCtrlCmdGrStaticGetFecsTraceDefines__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo__EXPORT(void *pSubdevice, void *pGpuInfoParams) {
    return subdeviceCtrlCmdGpuGetCachedInfo_IMPL(pSubdevice, pGpuInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdGpuForceGspUnload__EXPORT(void *pSubdevice, void *pGpuInfoParams) {
    return subdeviceCtrlCmdGpuForceGspUnload_IMPL(pSubdevice, pGpuInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00030118u)
static NV_STATUS subdeviceCtrlCmdGpuGetInfoV2__EXPORT(void *pSubdevice, void *pGpuInfoParams) {
    return subdeviceCtrlCmdGpuGetInfoV2_IMPL(pSubdevice, pGpuInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00030118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetIpVersion__EXPORT(void *pSubdevice, void *pGpuIpVersionParams) {
    return subdeviceCtrlCmdGpuGetIpVersion_IMPL(pSubdevice, pGpuIpVersionParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__EXPORT(void *pSubdevice, void *pBridgeInfoParams) {
    return subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL(pSubdevice, pBridgeInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__EXPORT(void *pSubdevice, void *pBridgeInfoParams) {
    return subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL(pSubdevice, pBridgeInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo__EXPORT(void *pSubdevice, void *pGpuOptimusInfoParams) {
    return subdeviceCtrlCmdGpuSetOptimusInfo_IMPL(pSubdevice, pGpuOptimusInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0002010au)
static NV_STATUS subdeviceCtrlCmdGpuGetNameString__EXPORT(void *pSubdevice, void *pNameStringParams) {
    return subdeviceCtrlCmdGpuGetNameString_IMPL(pSubdevice, pNameStringParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0002010au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000050au)
static NV_STATUS subdeviceCtrlCmdGpuGetShortNameString__EXPORT(void *pSubdevice, void *pShortNameStringParams) {
    return subdeviceCtrlCmdGpuGetShortNameString_IMPL(pSubdevice, pShortNameStringParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000050au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity__EXPORT(void *pSubdevice, void *pEncoderCapacityParams) {
    return subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL(pSubdevice, pEncoderCapacityParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__EXPORT(void *pSubdevice, void *params) {
    return subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL(pSubdevice, params);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__EXPORT(void *pSubdevice, void *params) {
    return subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL(pSubdevice, params);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuSetFabricAddr_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuSetPower__EXPORT(void *pSubdevice, void *pSetPowerParams) {
    return subdeviceCtrlCmdGpuSetPower_IMPL(pSubdevice, pSetPowerParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS subdeviceCtrlCmdGpuGetSdm__EXPORT(void *pSubdevice, void *pSdmParams) {
    return subdeviceCtrlCmdGpuGetSdm_IMPL(pSubdevice, pSdmParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000050bu)
static NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo__EXPORT(void *pSubdevice, void *pGpuSimulationInfoParams) {
    return subdeviceCtrlCmdGpuGetSimulationInfo_IMPL(pSubdevice, pGpuSimulationInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000050bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS subdeviceCtrlCmdGpuGetEngines__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetEngines_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2__EXPORT(void *pSubdevice, void *pEngineParams) {
    return subdeviceCtrlCmdGpuGetEnginesV2_IMPL(pSubdevice, pEngineParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist__EXPORT(void *pSubdevice, void *pClassParams) {
    return subdeviceCtrlCmdGpuGetEngineClasslist_IMPL(pSubdevice, pClassParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList__EXPORT(void *pSubdevice, void *pPartnerListParams) {
    return subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL(pSubdevice, pPartnerListParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo__EXPORT(void *pSubdevice, void *pGpuFermiZcullInfoParams) {
    return subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL(pSubdevice, pGpuFermiZcullInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetPesInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetPesInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdGpuExecRegOps__EXPORT(void *pSubdevice, void *pRegParams) {
    return subdeviceCtrlCmdGpuExecRegOps_IMPL(pSubdevice, pRegParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuMigratableOps__EXPORT(void *pSubdevice, void *pRegParams) {
    return subdeviceCtrlCmdGpuMigratableOps_IMPL(pSubdevice, pRegParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000448u)
static NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion__EXPORT(void *pSubdevice, void *pVersionInfo) {
    return subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL(pSubdevice, pVersionInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000448u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion__EXPORT(void *pSubdevice, void *pVersionInfo) {
    return subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL(pSubdevice, pVersionInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050158u)
static NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050158u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040448u)
static NV_STATUS subdeviceCtrlCmdGpuGetChipDetails__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetChipDetails_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040448u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000448u)
static NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo__EXPORT(void *pSubdevice, void *pBoardInfo) {
    return subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL(pSubdevice, pBoardInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000448u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000448u)
static NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo__EXPORT(void *pSubdevice, void *pOemInfo) {
    return subdeviceCtrlCmdGpuGetOEMInfo_IMPL(pSubdevice, pOemInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000448u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000003u)
static NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuHandleGpuSR_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000003u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000114u)
static NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules__EXPORT(void *pSubdevice, void *pSetRulesParams) {
    return subdeviceCtrlCmdGpuSetComputeModeRules_IMPL(pSubdevice, pSetRulesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000114u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules__EXPORT(void *pSubdevice, void *pQueryRulesParams) {
    return subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL(pSubdevice, pQueryRulesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014244u)
static NV_STATUS subdeviceCtrlCmdGpuInitializeCtx__EXPORT(void *pSubdevice, void *pInitializeCtxParams) {
    return subdeviceCtrlCmdGpuInitializeCtx_IMPL(pSubdevice, pInitializeCtxParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014244u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010244u)
static NV_STATUS subdeviceCtrlCmdGpuPromoteCtx__EXPORT(void *pSubdevice, void *pPromoteCtxParams) {
    return subdeviceCtrlCmdGpuPromoteCtx_IMPL(pSubdevice, pPromoteCtxParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010244u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c240u)
static NV_STATUS subdeviceCtrlCmdGpuEvictCtx__EXPORT(void *pSubdevice, void *pEvictCtxParams) {
    return subdeviceCtrlCmdGpuEvictCtx_IMPL(pSubdevice, pEvictCtxParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c240u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010au)
static NV_STATUS subdeviceCtrlCmdGpuGetId__EXPORT(void *pSubdevice, void *pIdParams) {
    return subdeviceCtrlCmdGpuGetId_IMPL(pSubdevice, pIdParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdGpuGetGidInfo__EXPORT(void *pSubdevice, void *pGidInfoParams) {
    return subdeviceCtrlCmdGpuGetGidInfo_IMPL(pSubdevice, pGidInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport__EXPORT(void *pSubdevice, void *pConfigParams) {
    return subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(pSubdevice, pConfigParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetIllum__EXPORT(void *pSubdevice, void *pConfigParams) {
    return subdeviceCtrlCmdGpuGetIllum_IMPL(pSubdevice, pConfigParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuSetIllum__EXPORT(void *pSubdevice, void *pConfigParams) {
    return subdeviceCtrlCmdGpuSetIllum_IMPL(pSubdevice, pConfigParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetVprCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetVprCaps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetVprInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetVprInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetPids__EXPORT(void *pSubdevice, void *pGetPidsParams) {
    return subdeviceCtrlCmdGpuGetPids_IMPL(pSubdevice, pGetPidsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetPidInfo__EXPORT(void *pSubdevice, void *pGetPidInfoParams) {
    return subdeviceCtrlCmdGpuGetPidInfo_IMPL(pSubdevice, pGetPidInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetSkylineInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetSkylineInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040040u)
static NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetHwEngineId_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010448u)
static NV_STATUS subdeviceCtrlCmdGpuGetFirstAsyncCEIdx__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010448u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000050bu)
static NV_STATUS subdeviceCtrlCmdGpuGetVmmuSegmentSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetVmmuSegmentSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000050bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010018u)
static NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdGpuHandleVfPriFault__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuHandleVfPriFault_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000107u)
static NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000107u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000110u)
static NV_STATUS subdeviceCtrlCmdValidateMemMapRequest__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdValidateMemMapRequest_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000110u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetGfid__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetGfid_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdUpdateGfidP2pCapability__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000000au)
static NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000000au)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS subdeviceCtrlCmdGetP2pCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetP2pCaps_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGetGpuFabricProbeInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)
static NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForReset__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuMarkDeviceForReset_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)
static NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForReset__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuUnmarkDeviceForReset_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)
static NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)
static NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)
static NV_STATUS subdeviceCtrlCmdGpuGetResetStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetResetStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetDrainAndResetStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetDrainAndResetStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS subdeviceCtrlCmdGpuGetConstructedFalconInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetConstructedFalconInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlGpuGetFipsStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlGpuGetFipsStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS subdeviceCtrlCmdGpuGetVfCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetVfCaps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS subdeviceCtrlCmdGpuGetRecoveryAction__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetRecoveryAction_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100108u)
static NV_STATUS subdeviceCtrlCmdGpuRpcGspTest__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuRpcGspTest_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100108u)
static NV_STATUS subdeviceCtrlCmdGpuRpcGspQuerySizes__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuRpcGspQuerySizes_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS subdeviceCtrlCmdRusdGetSupportedFeatures__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdRusdGetSupportedFeatures_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000014u)
static NV_STATUS subdeviceCtrlCmdRusdSetFeatures__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdRusdSetFeatures_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000014u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100004u)
static NV_STATUS subdeviceCtrlCmdGpuErrorInjectionControl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuErrorInjectionControl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdGpuCheckMemSubsysError__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuCheckMemSubsysError_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS subdeviceCtrlCmdGpuGetDefaultTimeout__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetDefaultTimeout_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100044u)
static NV_STATUS subdeviceCtrlCmdGspCryptoControl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGspCryptoControl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuIsResetCoupled__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuIsResetCoupled_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetDieletInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetDieletInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGpuGetUnitFsInfoFromChiplet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetUnitFsInfoFromChiplet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000208u)
static NV_STATUS subdeviceCtrlCmdGpuSetMigrationBlock__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuSetMigrationBlock_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000208u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS subdeviceCtrlCmdGpuGetEgmInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetEgmInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdEventSetTrigger__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdEventSetTrigger_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo__EXPORT(void *pSubdevice, void *pTriggerFifoParams) {
    return subdeviceCtrlCmdEventSetTriggerFifo_IMPL(pSubdevice, pTriggerFifoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdEventSetNotification__EXPORT(void *pSubdevice, void *pSetEventParams) {
    return subdeviceCtrlCmdEventSetNotification_IMPL(pSubdevice, pSetEventParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies__EXPORT(void *pSubdevice, void *pSetMemoryNotifiesParams) {
    return subdeviceCtrlCmdEventSetMemoryNotifies_IMPL(pSubdevice, pSetMemoryNotifiesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory__EXPORT(void *pSubdevice, void *pSetSemMemoryParams) {
    return subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL(pSubdevice, pSetSemMemoryParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation__EXPORT(void *pSubdevice, void *pSetSemaMemValidationParams) {
    return subdeviceCtrlCmdEventSetSemaMemValidation_IMPL(pSubdevice, pSetSemaMemValidationParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdEventVideoBindEvtbuf__EXPORT(void *pSubdevice, void *pBindParams) {
    return subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL(pSubdevice, pBindParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdEventNocatBindEvtbuf__EXPORT(void *pSubdevice, void *pBindParams) {
    return subdeviceCtrlCmdEventNocatBindEvtbuf_IMPL(pSubdevice, pBindParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf__EXPORT(void *pSubdevice, void *pBindParams) {
    return subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL(pSubdevice, pBindParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdTimerCancel__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdTimerCancel_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdTimerSchedule__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdTimerSchedule_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdTimerGetTime__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdTimerGetTime_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset__EXPORT(void *pSubdevice, void *pTimerRegOffsetParams) {
    return subdeviceCtrlCmdTimerGetRegisterOffset_IMPL(pSubdevice, pTimerRegOffsetParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010208u)
static NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdTimerSetGrTickFreq_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010208u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS subdeviceCtrlCmdRcReadVirtualMem__EXPORT(void *pSubdevice, void *pReadVirtMemParam) {
    return subdeviceCtrlCmdRcReadVirtualMem_IMPL(pSubdevice, pReadVirtMemParam);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdRcGetErrorCount__EXPORT(void *pSubdevice, void *pErrorCount) {
    return subdeviceCtrlCmdRcGetErrorCount_IMPL(pSubdevice, pErrorCount);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdRcGetErrorV2__EXPORT(void *pSubdevice, void *pErrorParams) {
    return subdeviceCtrlCmdRcGetErrorV2_IMPL(pSubdevice, pErrorParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo__EXPORT(void *pSubdevice, void *pWatchdogInfoParams) {
    return subdeviceCtrlCmdRcGetWatchdogInfo_IMPL(pSubdevice, pWatchdogInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)
static NV_STATUS subdeviceCtrlCmdRcDisableWatchdog__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdRcDisableWatchdog_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)
static NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)
static NV_STATUS subdeviceCtrlCmdRcEnableWatchdog__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdRcEnableWatchdog_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000014u)
static NV_STATUS subdeviceCtrlCmdRcPauseWatchdog__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdRcPauseWatchdog_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000014u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)
static NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000018u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040154u)
static NV_STATUS subdeviceCtrlCmdSetRcRecovery__EXPORT(void *pSubdevice, void *pRcRecovery) {
    return subdeviceCtrlCmdSetRcRecovery_DISPATCH(pSubdevice, pRcRecovery);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040154u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040154u)
static NV_STATUS subdeviceCtrlCmdGetRcRecovery__EXPORT(void *pSubdevice, void *pRcRecovery) {
    return subdeviceCtrlCmdGetRcRecovery_DISPATCH(pSubdevice, pRcRecovery);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040154u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdGetRcInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetRcInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdSetRcInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdSetRcInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdSetRcWatchdogInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdSetRcWatchdogInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdNvdGetDumpSize__EXPORT(void *pSubdevice, void *pDumpSizeParams) {
    return subdeviceCtrlCmdNvdGetDumpSize_IMPL(pSubdevice, pDumpSizeParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdNvdGetDump__EXPORT(void *pSubdevice, void *pDumpParams) {
    return subdeviceCtrlCmdNvdGetDump_IMPL(pSubdevice, pDumpParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt__EXPORT(void *pSubdevice, void *pReportParams) {
    return subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL(pSubdevice, pReportParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData__EXPORT(void *pSubdevice, void *pReportParams) {
    return subdeviceCtrlCmdNvdSetNocatJournalData_IMPL(pSubdevice, pReportParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS subdeviceCtrlCmdNvdInsertNocatJournalRecord__EXPORT(void *pSubdevice, void *pReportParams) {
    return subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL(pSubdevice, pReportParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)
static NV_STATUS subdeviceCtrlCmdPmgrGetModuleInfo__EXPORT(void *pSubdevice, void *pModuleInfoParams) {
    return subdeviceCtrlCmdPmgrGetModuleInfo_IMPL(pSubdevice, pModuleInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000002u)
static NV_STATUS subdeviceCtrlCmdGc6Entry__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGc6Entry_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000002u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000002u)
static NV_STATUS subdeviceCtrlCmdGc6Exit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGc6Exit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000002u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS subdeviceCtrlCmdLpwrDifrPrefetchResponse__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdLpwrDifrPrefetchResponse_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdLpwrDifrCtrl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdLpwrDifrCtrl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdCeGetCaps__EXPORT(void *pSubdevice, void *pCeCapsParams) {
    return subdeviceCtrlCmdCeGetCaps_DISPATCH(pSubdevice, pCeCapsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00030349u)
static NV_STATUS subdeviceCtrlCmdCeGetCePceMask__EXPORT(void *pSubdevice, void *pCePceMaskParams) {
    return subdeviceCtrlCmdCeGetCePceMask_IMPL(pSubdevice, pCePceMaskParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00030349u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappings__EXPORT(void *pSubdevice, void *pCeUpdatePceLceMappingsParams) {
    return subdeviceCtrlCmdCeUpdatePceLceMappings_KERNEL(pSubdevice, pCeUpdatePceLceMappingsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappingsV2__EXPORT(void *pSubdevice, void *pCeUpdatePceLceMappingsParams) {
    return subdeviceCtrlCmdCeUpdatePceLceMappingsV2_KERNEL(pSubdevice, pCeUpdatePceLceMappingsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000149u)
static NV_STATUS subdeviceCtrlCmdCeGetLceShimInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeGetLceShimInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000149u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdCeGetPceConfigForLceType__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeGetPceConfigForLceType_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000102c0u)
static NV_STATUS subdeviceCtrlCmdCeGetPceConfigForLceMIGGpuInstance__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeGetPceConfigForLceMIGGpuInstance_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000102c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdCeApplyPceLceMappingForMIGGpuInstance__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeApplyPceLceMappingForMIGGpuInstance_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS subdeviceCtrlCmdCeGetCapsV2__EXPORT(void *pSubdevice, void *pCeCapsParams) {
    return subdeviceCtrlCmdCeGetCapsV2_DISPATCH(pSubdevice, pCeCapsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS subdeviceCtrlCmdCeGetAllCaps__EXPORT(void *pSubdevice, void *pCeCapsParams) {
    return subdeviceCtrlCmdCeGetAllCaps_DISPATCH(pSubdevice, pCeCapsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000154u)
static NV_STATUS subdeviceCtrlCmdCeGetDecompLceMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeGetDecompLceMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000154u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000154u)
static NV_STATUS subdeviceCtrlCmdCeIsDecompLceEnabled__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeIsDecompLceEnabled_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000154u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage__EXPORT(void *pSubdevice, void *pFlcnDmemUsageParams) {
    return subdeviceCtrlCmdFlcnGetDmemUsage_IMPL(pSubdevice, pFlcnDmemUsageParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlcnGetEngineArch_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccGetClientExposedCounters_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdEccGetVolatileCounts__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccGetVolatileCounts_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdEccInjectError__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccInjectError_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdEccGetRepairStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccGetRepairStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdEccInjectionSupported__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccInjectionSupported_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdGpuQueryEccConfiguration__EXPORT(void *pSubdevice, void *pConfig) {
    return subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(pSubdevice, pConfig);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040044u)
static NV_STATUS subdeviceCtrlCmdGpuSetEccConfiguration__EXPORT(void *pSubdevice, void *pConfig) {
    return subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(pSubdevice, pConfig);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040044u)
static NV_STATUS subdeviceCtrlCmdGpuResetEccErrorStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdEccGetSramUniqueUncorrCounts__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccGetSramUniqueUncorrCounts_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdEccGetUnrepairableMemoryFlag__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdEccGetUnrepairableMemoryFlag_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdFlaRange__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlaRange_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010244u)
static NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010244u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010004u)
static NV_STATUS subdeviceCtrlCmdFlaGetRange__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlaGetRange_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdFlaGetFabricMemStats_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040549u)
static NV_STATUS subdeviceCtrlCmdGspGetFeatures__EXPORT(void *pSubdevice, void *pGspFeaturesParams) {
    return subdeviceCtrlCmdGspGetFeatures_DISPATCH(pSubdevice, pGspFeaturesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040549u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS subdeviceCtrlCmdGspGetRmHeapStats__EXPORT(void *pSubdevice, void *pGspRmHeapStatsParams) {
    return subdeviceCtrlCmdGspGetRmHeapStats_IMPL(pSubdevice, pGspRmHeapStatsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000204u)
static NV_STATUS subdeviceCtrlCmdGpuGetVgpuHeapStats__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetVgpuHeapStats_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000204u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000248u)
static NV_STATUS subdeviceCtrlCmdLibosGetHeapStats__EXPORT(void *pSubdevice, void *pGspLibosHeapStatsParams) {
    return subdeviceCtrlCmdLibosGetHeapStats_IMPL(pSubdevice, pGspLibosHeapStatsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100004u)
static NV_STATUS subdeviceCtrlCmdGspGdmaFuzzTest__EXPORT(void *pSubdevice, void *pGspGdmaFuzzTestParams) {
    return subdeviceCtrlCmdGspGdmaFuzzTest_IMPL(pSubdevice, pGspGdmaFuzzTestParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100044u)
static NV_STATUS subdeviceCtrlCmdGspCoreTest__EXPORT(void *pSubdevice, void *pGspCoreTestParams) {
    return subdeviceCtrlCmdGspCoreTest_395e98(pSubdevice, pGspCoreTestParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuDescribePartitions__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuDescribePartitions_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuSetPartitioningMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuSetPartitions__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuSetPartitions_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)
static NV_STATUS subdeviceCtrlCmdGpuGetPartitions__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetPartitions_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS subdeviceCtrlCmdGpuGetComputeProfiles__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetComputeProfiles_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdGpuGetComputeProfileCapacity__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGpuGetComputeProfileCapacity_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000080u)
static NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000080u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000080u)
static NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000080u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000003u)
static NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000003u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000001u)
static NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000001u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000000bu)
static NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000000bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayGetIpVersion_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayGetStaticInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayWriteInstMem_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplaySetImportedImpData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayGetDisplayMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayPinsetsToLockpins__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayPinsetsToLockpins_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGpioProgramDirection__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGpioProgramDirection_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGpioProgramOutput__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGpioProgramOutput_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGpioReadInput__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGpioReadInput_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGpioActivateHwFunction__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGpioActivateHwFunction_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalDisplayPreModeSet__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalDisplayPreModeSet_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalDisplayPostModeSet__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalDisplayPostModeSet_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayPreUnixConsole__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayPreUnixConsole_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdDisplayPostUnixConsole__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdDisplayPostUnixConsole_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000004c0u)
static NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000004c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c4c0u)
static NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c4c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGetSmcMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGetSmcMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000002c0u)
static NV_STATUS subdeviceCtrlCmdInternalCheckCtsIdValid__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalCheckCtsIdValid_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000002c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdIsEgpuBridge__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdIsEgpuBridge_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGpuGetPFBar1Spa__EXPORT(void *pSubdevice, void *pConfigParams) {
    return subdeviceCtrlCmdInternalGpuGetPFBar1Spa_IMPL(pSubdevice, pConfigParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGmmuGetStaticInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer__EXPORT(void *pSubdevice, void *pCopyServerReservedPdesParams) {
    return subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_IMPL(pSubdevice, pCopyServerReservedPdesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000101d0u)
static NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps__EXPORT(void *pSubdevice, void *pCeCapsParams) {
    return subdeviceCtrlCmdCeGetPhysicalCaps_IMPL(pSubdevice, pCeCapsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000101d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000101d0u)
static NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps__EXPORT(void *pSubdevice, void *pCeCapsParams) {
    return subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL(pSubdevice, pCeCapsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000101d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdCeUpdateClassDB__EXPORT(void *pSubdevice, void *params) {
    return subdeviceCtrlCmdCeUpdateClassDB_IMPL(pSubdevice, params);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c040u)
static NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize__EXPORT(void *pSubdevice, void *params) {
    return subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL(pSubdevice, params);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0001c040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000004c0u)
static NV_STATUS subdeviceCtrlCmdCeGetHubPceMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeGetHubPceMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000004c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS subdeviceCtrlCmdCeGetHubPceMaskV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCeGetHubPceMaskV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdIntrGetKernelTable__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdIntrGetKernelTable_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)
static NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)
static NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)
static NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfGetAuxPowerState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfGetAuxPowerState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfClientPerfModesSetControl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfClientPerfModesSetControl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGcxEntryPrerequisite__EXPORT(void *pSubdevice, void *pGcxEntryPrerequisite) {
    return subdeviceCtrlCmdInternalGcxEntryPrerequisite_IMPL(pSubdevice, pGcxEntryPrerequisite);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdBifGetStaticInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalBifGetData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalBifGetData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdBifSetPcieRo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBifSetPcieRo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdBifDisableSystemMemoryAccess__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBifDisableSystemMemoryAccess_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdHshubPeerConnConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdHshubGetNumUnits__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdHshubGetNumUnits_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdHshubNextHshubId__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdHshubNextHshubId_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdHshubGetMaxHshubsPerShim__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdHshubGetMaxHshubsPerShim_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdHshubEgmConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdHshubEgmConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkLfmRmAction__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkLfmRmAction_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c1u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c1u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkCoreCallback__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkCoreCallback_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetAliEnabled__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetAliEnabled_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramBufferready__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkProgramBufferready_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c1u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c1u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c1u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c1u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateHshubMux__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkLinkTrainAli__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkLinkTrainAli_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkAreLinksTrained__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkAreLinksTrained_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkResetLinks__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkResetLinks_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkRCUserModeChannels__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalNvlinkRCUserModeChannels_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkResumeDisableChannels__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkResumeDisableChannels_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalNvlinkForceDebugStallAndUpdateAmap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalNvlinkForceDebugStallAndUpdateAmap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalSetP2pCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalSetP2pCaps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalRemoveP2pCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalRemoveP2pCaps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGetLocalAtsConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalSetPeerAtsConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalInitGpuIntr__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalInitGpuIntr_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncOptimizeTiming__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncOptimizeTiming_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncGetDisplayIds__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncGetDisplayIds_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncSetStereoSync__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncSetStereoSync_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncGetVactiveLines__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncGetVactiveLines_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncIsDisplayIdValid__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar__EXPORT(void *pSubdevice, void *pAttachParams) {
    return subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_IMPL(pSubdevice, pAttachParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalFbsrInit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalFbsrInit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPostInitBrightcStateLoad__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPostInitBrightcStateLoad_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalSetStaticEdidData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalSetStaticEdidData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalDetectHsVideoBridge__EXPORT(void *pSubdevice) {
    return subdeviceCtrlCmdInternalDetectHsVideoBridge_IMPL(pSubdevice);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeGetStaticInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeGetStaticInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveLceKeys__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeRotateKeys__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeRotateKeys_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeRotatePerChannelKeys__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeRotatePerChannelKeys_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeSetGpuState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeSetGpuState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalInitUserSharedData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalInitUserSharedData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalUserSharedDataSetDataPoll__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000102d0u)
static NV_STATUS subdeviceCtrlCmdInternalControlGspTrace__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalControlGspTrace_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000102d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalGpuSetIllum__EXPORT(void *pSubdevice, void *pConfigParams) {
    return subdeviceCtrlCmdInternalGpuSetIllum_IMPL(pSubdevice, pConfigParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalLogOobXid__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalLogOobXid_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS subdeviceCtrlCmdInternalGpuGetHfrpInfo__EXPORT(void *pSubdevice, void *pHfrpParams) {
    return subdeviceCtrlCmdInternalGpuGetHfrpInfo_IMPL(pSubdevice, pHfrpParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalUcodeInstrumentationGetState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalUcodeInstrumentationGetState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalUcodeInstrumentationSetState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalUcodeInstrumentationSetState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdInternalUcodeInstrumentationGetData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdInternalUcodeInstrumentationGetData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000001d0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000001d0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalFreeStates__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalFreeStates_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalScrubGuestFb__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalScrubGuestFb_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalCheckScrubComplete__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalCheckScrubComplete_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalAllocGspCeUtils__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalAllocGspCeUtils_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdVgpuMgrInternalFreeGspCeUtils__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdVgpuMgrInternalFreeGspCeUtils_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)
static NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdGetAvailableHshubMask_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)
static NV_STATUS subdeviceCtrlSetEcThrottleMode__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlSetEcThrottleMode_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000158u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdCcuMap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCcuMap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdCcuUnmap__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCcuUnmap_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdCcuSetStreamState__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCcuSetStreamState_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)
static NV_STATUS subdeviceCtrlCmdCcuGetSampleInfo__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000400c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)
static NV_STATUS subdeviceCtrlCmdSpdmPartition__EXPORT(void *pSubdevice, void *pSpdmPartitionParams) {
    return subdeviceCtrlCmdSpdmPartition_IMPL(pSubdevice, pSpdmPartitionParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c0u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceSpdmRetrieveTranscript__EXPORT(void *pSubdevice, void *pSpdmRetrieveSessionTranscriptParams) {
    return subdeviceSpdmRetrieveTranscript_IMPL(pSubdevice, pSpdmRetrieveSessionTranscriptParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS subdeviceCtrlCmdBifGetData__EXPORT(void *pSubdevice, void *pParams) {
    return subdeviceCtrlCmdBifGetData_IMPL(pSubdevice, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_Subdevice[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x30118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x30118u)
        /*flags=*/      0x30118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInfoV2"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetNameString__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010au)
        /*flags=*/      0x2010au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800110u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNameString"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetShortNameString__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50au)
        /*flags=*/      0x50au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800111u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetShortNameString"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetPower__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPower"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetSdm__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SDM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSdm"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetSimulationInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*flags=*/      0x50bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800119u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSimulationInfo"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuExecRegOps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuExecRegOps"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEngines__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngines"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEngineClasslist__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineClasslist"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEngineFaultInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800125u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineFaultInfo"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuPromoteCtx__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*flags=*/      0x10244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuPromoteCtx"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c240u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuEvictCtx__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c240u)
        /*flags=*/      0x1c240u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_EVICT_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuEvictCtx"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuInitializeCtx__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14244u)
        /*flags=*/      0x14244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuInitializeCtx"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryEccStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50158u)
        /*flags=*/      0x50158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryEccStatus"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetComputeModeRules__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
        /*flags=*/      0x114u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800130u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetComputeModeRules"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryComputeModeRules__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800131u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryComputeModeRules"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryEccConfiguration__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800133u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryEccConfiguration"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetEccConfiguration__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*flags=*/      0x40044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800134u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetEccConfiguration"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuResetEccErrorStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*flags=*/      0x40044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800136u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuResetEccErrorStatus"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetFermiGpcInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800137u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiGpcInfo"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetFermiTpcInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800138u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiTpcInfo"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetFermiZcullInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800139u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiZcullInfo"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetOEMBoardInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080013fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetOEMBoardInfo"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*flags=*/      0x10au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800142u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetId"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuAcquireComputeModeReservation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800145u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuAcquireComputeModeReservation"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuReleaseComputeModeReservation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800146u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuReleaseComputeModeReservation"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEnginePartnerList__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800147u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEnginePartnerList"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetGidInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetGidInfo"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetInforomObjectVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInforomObjectVersion"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetOptimusInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetOptimusInfo"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetIpVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetIpVersion"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryIllumSupport__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800153u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryIllumSupport"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetIllum__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800154u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetIllum"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetIllum__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800155u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetIllum"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetInforomImageVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800156u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInforomImageVersion"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryInforomEccSupport__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800157u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryInforomEccSupport"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryScrubberStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryScrubberStatus"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetVprCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800160u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVprCaps"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuHandleGpuSR__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*flags=*/      0x3u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800167u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuHandleGpuSR"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetPesInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800168u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PES_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPesInfo"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetOEMInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800169u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetOEMInfo"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetVprInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVprInfo"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEncoderCapacity__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEncoderCapacity"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetNvencSwSessionStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionStats"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetNvencSwSessionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionInfo"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetFabricAddr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetFabricAddr"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEnginesV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800170u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEnginesV2"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuQueryFunctionStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800173u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryFunctionStatus"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetPartitions__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800174u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPartitions"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetPartitions__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800175u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPartitions"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuReportNonReplayableFault__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
        /*flags=*/      0x40040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800177u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuReportNonReplayableFault"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEngineRunlistPriBase__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800179u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineRunlistPriBase"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetHwEngineId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetHwEngineId"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvfbcSwSessionStats"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetVmmuSegmentSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*flags=*/      0x50bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVmmuSegmentSize"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetPartitionCapacity__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800181u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPartitionCapacity"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetCachedInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800182u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetCachedInfo"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetPartitioningMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800183u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPartitioningMode"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuDescribePartitions__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800185u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuDescribePartitions"
#endif
    },
    {               /*  [62] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10018u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetMaxSupportedPageSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10018u)
        /*flags=*/      0x10018u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800188u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetMaxSupportedPageSize"
#endif
    },
    {               /*  [63] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetActivePartitionIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetActivePartitionIds"
#endif
    },
    {               /*  [64] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetPids__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PIDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPids"
#endif
    },
    {               /*  [65] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetPidInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPidInfo"
#endif
    },
    {               /*  [66] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuHandleVfPriFault__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800192u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuHandleVfPriFault"
#endif
    },
    {               /*  [67] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetComputePolicyConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
        /*flags=*/      0x107u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800194u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetComputePolicyConfig"
#endif
    },
    {               /*  [68] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetComputePolicyConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800195u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetComputePolicyConfig"
#endif
    },
    {               /*  [69] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetGfid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800196u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_GFID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetGfid"
#endif
    },
    {               /*  [70] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdUpdateGfidP2pCapability__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800197u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdUpdateGfidP2pCapability"
#endif
    },
    {               /*  [71] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdValidateMemMapRequest__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*flags=*/      0x110u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800198u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdValidateMemMapRequest"
#endif
    },
    {               /*  [72] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEngineLoadTimes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*flags=*/      0xau,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080019bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineLoadTimes"
#endif
    },
    {               /*  [73] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetSkylineInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080019fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SKYLINE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSkylineInfo"
#endif
    },
    {               /*  [74] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetP2pCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GET_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetP2pCaps"
#endif
    },
    {               /*  [75] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetComputeProfiles__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetComputeProfiles"
#endif
    },
    {               /*  [76] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetGpuFabricProbeInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetGpuFabricProbeInfo"
#endif
    },
    {               /*  [77] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetChipDetails__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*flags=*/      0x40448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetChipDetails"
#endif
    },
    {               /*  [78] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuMigratableOps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuMigratableOps"
#endif
    },
    {               /*  [79] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuMarkDeviceForReset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*flags=*/      0x100048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a9u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuMarkDeviceForReset"
#endif
    },
    {               /*  [80] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuUnmarkDeviceForReset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*flags=*/      0x100048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001aau,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuUnmarkDeviceForReset"
#endif
    },
    {               /*  [81] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetResetStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001abu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetResetStatus"
#endif
    },
    {               /*  [82] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*flags=*/      0x100048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001acu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset"
#endif
    },
    {               /*  [83] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*flags=*/      0x100048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001adu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset"
#endif
    },
    {               /*  [84] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetDrainAndResetStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001aeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetDrainAndResetStatus"
#endif
    },
    {               /*  [85] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001afu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2"
#endif
    },
    {               /*  [86] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetConstructedFalconInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001b0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetConstructedFalconInfo"
#endif
    },
    {               /*  [87] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetVfCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001b1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVfCaps"
#endif
    },
    {               /*  [88] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetRecoveryAction__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001b2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetRecoveryAction"
#endif
    },
    {               /*  [89] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x102d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalControlGspTrace__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x102d0u)
        /*flags=*/      0x102d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalControlGspTrace"
#endif
    },
    {               /*  [90] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlGpuGetFipsStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlGpuGetFipsStatus"
#endif
    },
    {               /*  [91] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetComputeProfileCapacity__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_COMPUTE_PROFILE_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetComputeProfileCapacity"
#endif
    },
    {               /*  [92] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetFirstAsyncCEIdx__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10448u)
        /*flags=*/      0x10448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FIRST_ASYNC_CE_IDX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFirstAsyncCEIdx"
#endif
    },
    {               /*  [93] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuRpcGspTest__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100108u)
        /*flags=*/      0x100108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuRpcGspTest"
#endif
    },
    {               /*  [94] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuRpcGspQuerySizes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100108u)
        /*flags=*/      0x100108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_RPC_GSP_QUERY_SIZES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuRpcGspQuerySizes"
#endif
    },
    {               /*  [95] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRusdGetSupportedFeatures__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001eau,
        /*paramSize=*/  sizeof(NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRusdGetSupportedFeatures"
#endif
    },
    {               /*  [96] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRusdSetFeatures__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*flags=*/      0x14u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001ebu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_RUSD_SET_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRusdSetFeatures"
#endif
    },
    {               /*  [97] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuForceGspUnload__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001ecu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_FORCE_GSP_UNLOAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuForceGspUnload"
#endif
    },
    {               /*  [98] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuErrorInjectionControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100004u)
        /*flags=*/      0x100004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001efu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_ERROR_INJECTION_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuErrorInjectionControl"
#endif
    },
    {               /*  [99] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuCheckMemSubsysError__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001f0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_CHECK_MEM_SUBSYS_ERROR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuCheckMemSubsysError"
#endif
    },
    {               /*  [100] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetDefaultTimeout__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001f1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_DEFAULT_TIMEOUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetDefaultTimeout"
#endif
    },
    {               /*  [101] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGspCryptoControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100044u)
        /*flags=*/      0x100044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001f2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GSP_CRYPTO_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspCryptoControl"
#endif
    },
    {               /*  [102] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuSetMigrationBlock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*flags=*/      0x208u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001fau,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_MIGRATION_BLOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetMigrationBlock"
#endif
    },
    {               /*  [103] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetEgmInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001fbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_EGM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEgmInfo"
#endif
    },
    {               /*  [104] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventSetNotification__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800301u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetNotification"
#endif
    },
    {               /*  [105] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventSetTrigger__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800302u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetTrigger"
#endif
    },
    {               /*  [106] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventSetMemoryNotifies__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetMemoryNotifies"
#endif
    },
    {               /*  [107] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventSetSemaphoreMemory__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800304u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetSemaphoreMemory"
#endif
    },
    {               /*  [108] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventSetSemaMemValidation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800306u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetSemaMemValidation"
#endif
    },
    {               /*  [109] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventSetTriggerFifo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800308u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetTriggerFifo"
#endif
    },
    {               /*  [110] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventVideoBindEvtbuf__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800309u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventVideoBindEvtbuf"
#endif
    },
    {               /*  [111] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080030au,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf"
#endif
    },
    {               /*  [112] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEventNocatBindEvtbuf__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080030bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_NOCAT_BIND_EVTBUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventNocatBindEvtbuf"
#endif
    },
    {               /*  [113] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdTimerSchedule__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerSchedule"
#endif
    },
    {               /*  [114] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdTimerCancel__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800402u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerCancel"
#endif
    },
    {               /*  [115] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdTimerGetTime__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800403u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetTime"
#endif
    },
    {               /*  [116] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdTimerGetRegisterOffset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800404u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetRegisterOffset"
#endif
    },
    {               /*  [117] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800406u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo"
#endif
    },
    {               /*  [118] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10208u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdTimerSetGrTickFreq__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10208u)
        /*flags=*/      0x10208u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800407u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerSetGrTickFreq"
#endif
    },
    {               /*  [119] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdThermalSystemExecuteV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800513u,
        /*paramSize=*/  sizeof(NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdThermalSystemExecuteV2"
#endif
    },
    {               /*  [120] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdI2cReadBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800601u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_READ_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cReadBuffer"
#endif
    },
    {               /*  [121] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdI2cWriteBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800602u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cWriteBuffer"
#endif
    },
    {               /*  [122] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdI2cReadReg__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800603u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_RW_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cReadReg"
#endif
    },
    {               /*  [123] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdI2cWriteReg__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800604u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_RW_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cWriteReg"
#endif
    },
    {               /*  [124] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBiosGetSKUInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000au)
        /*flags=*/      0x1000au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800808u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetSKUInfo"
#endif
    },
    {               /*  [125] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBiosGetPostTime__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800809u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetPostTime"
#endif
    },
    {               /*  [126] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBiosGetUefiSupport__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080080bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetUefiSupport"
#endif
    },
    {               /*  [127] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBiosGetNbsiV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080080eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetNbsiV2"
#endif
    },
    {               /*  [128] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x60048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBiosGetInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x60048u)
        /*flags=*/      0x60048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800810u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetInfoV2"
#endif
    },
    {               /*  [129] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBifGetData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a00u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIF_GET_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBifGetData"
#endif
    },
    {               /*  [130] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayGetStaticInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetStaticInfo"
#endif
    },
    {               /*  [131] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a14u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode"
#endif
    },
    {               /*  [132] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysGetStaticConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c0c0u)
        /*flags=*/      0x5c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysGetStaticConfig"
#endif
    },
    {               /*  [133] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer"
#endif
    },
    {               /*  [134] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer"
#endif
    },
    {               /*  [135] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetCaps"
#endif
    },
    {               /*  [136] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc1u)
        /*flags=*/      0xc1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a21u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer"
#endif
    },
    {               /*  [137] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a22u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder"
#endif
    },
    {               /*  [138] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkCoreCallback__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a24u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkCoreCallback"
#endif
    },
    {               /*  [139] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a25u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid"
#endif
    },
    {               /*  [140] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a26u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks"
#endif
    },
    {               /*  [141] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCtxBufferPtes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a28u,
        /*paramSize=*/  sizeof(NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferPtes"
#endif
    },
    {               /*  [142] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetAliEnabled__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a29u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetAliEnabled"
#endif
    },
    {               /*  [143] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetInfo"
#endif
    },
    {               /*  [144] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetZcullInfo"
#endif
    },
    {               /*  [145] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetRopInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetRopInfo"
#endif
    },
    {               /*  [146] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a30u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetPpcMasks"
#endif
    },
    {               /*  [147] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c1c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c1c0u)
        /*flags=*/      0x1c1c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a32u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo"
#endif
    },
    {               /*  [148] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a34u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier"
#endif
    },
    {               /*  [149] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a37u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable"
#endif
    },
    {               /*  [150] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a38u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable"
#endif
    },
    {               /*  [151] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a39u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset"
#endif
    },
    {               /*  [152] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset"
#endif
    },
    {               /*  [153] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset"
#endif
    },
    {               /*  [154] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize"
#endif
    },
    {               /*  [155] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrStaticGetFecsTraceDefines__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10040u)
        /*flags=*/      0x10040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrStaticGetFecsTraceDefines"
#endif
    },
    {               /*  [156] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines"
#endif
    },
    {               /*  [157] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c4c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGetDeviceInfoTable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c4c0u)
        /*flags=*/      0x1c4c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a40u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetDeviceInfoTable"
#endif
    },
    {               /*  [158] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGetUserRegisterAccessMap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*flags=*/      0x4c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a41u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetUserRegisterAccessMap"
#endif
    },
    {               /*  [159] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkUpdateHshubMux__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a42u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkUpdateHshubMux"
#endif
    },
    {               /*  [160] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a44u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange"
#endif
    },
    {               /*  [161] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a45u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck"
#endif
    },
    {               /*  [162] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalCheckCtsIdValid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2c0u)
        /*flags=*/      0x2c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a46u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_CHECK_CTS_ID_VALID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalCheckCtsIdValid"
#endif
    },
    {               /*  [163] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a48u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetPdbProperties"
#endif
    },
    {               /*  [164] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayWriteInstMem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a49u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayWriteInstMem"
#endif
    },
    {               /*  [165] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalRecoverAllComputeContexts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4au,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRecoverAllComputeContexts"
#endif
    },
    {               /*  [166] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayGetIpVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetIpVersion"
#endif
    },
    {               /*  [167] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGetSmcMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetSmcMode"
#endif
    },
    {               /*  [168] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplaySetupRgLineIntr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetupRgLineIntr"
#endif
    },
    {               /*  [169] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer"
#endif
    },
    {               /*  [170] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a50u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer"
#endif
    },
    {               /*  [171] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysSetPartitionableMem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a51u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysSetPartitionableMem"
#endif
    },
    {               /*  [172] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a53u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers"
#endif
    },
    {               /*  [173] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplaySetImportedImpData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a54u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetImportedImpData"
#endif
    },
    {               /*  [174] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdIsEgpuBridge__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a55u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIsEgpuBridge"
#endif
    },
    {               /*  [175] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalLogOobXid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a56u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalLogOobXid"
#endif
    },
    {               /*  [176] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a57u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries"
#endif
    },
    {               /*  [177] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplaySetChannelPushbuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a58u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetChannelPushbuffer"
#endif
    },
    {               /*  [178] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGmmuGetStaticInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a59u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGmmuGetStaticInfo"
#endif
    },
    {               /*  [179] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetHeapReservationSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetHeapReservationSize"
#endif
    },
    {               /*  [180] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdIntrGetKernelTable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIntrGetKernelTable"
#endif
    },
    {               /*  [181] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayGetDisplayMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetDisplayMask"
#endif
    },
    {               /*  [182] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping"
#endif
    },
    {               /*  [183] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalFifoGetNumChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
        /*flags=*/      0x1d8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a61u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoGetNumChannels"
#endif
    },
    {               /*  [184] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a62u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState"
#endif
    },
    {               /*  [185] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a63u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles"
#endif
    },
    {               /*  [186] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkProgramBufferready__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a64u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkProgramBufferready"
#endif
    },
    {               /*  [187] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a65u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines"
#endif
    },
    {               /*  [188] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a66u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges"
#endif
    },
    {               /*  [189] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a67u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKMemSysGetMIGMemoryConfig"
#endif
    },
    {               /*  [190] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbSetZbcReferenced__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a69u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetZbcReferenced"
#endif
    },
    {               /*  [191] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalRcWatchdogTimeout__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6au,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRcWatchdogTimeout"
#endif
    },
    {               /*  [192] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable"
#endif
    },
    {               /*  [193] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysL2InvalidateEvict__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysL2InvalidateEvict"
#endif
    },
    {               /*  [194] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6du,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches"
#endif
    },
    {               /*  [195] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysDisableNvlinkPeers__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6eu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysDisableNvlinkPeers"
#endif
    },
    {               /*  [196] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysProgramRawCompressionMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysProgramRawCompressionMode"
#endif
    },
    {               /*  [197] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBusFlushWithSysmembar__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a70u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusFlushWithSysmembar"
#endif
    },
    {               /*  [198] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a71u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal"
#endif
    },
    {               /*  [199] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a72u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote"
#endif
    },
    {               /*  [200] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBusDestroyP2pMailbox__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a73u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusDestroyP2pMailbox"
#endif
    },
    {               /*  [201] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a74u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusCreateC2cPeerMapping"
#endif
    },
    {               /*  [202] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a75u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping"
#endif
    },
    {               /*  [203] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayPreUnixConsole__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a76u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayPreUnixConsole"
#endif
    },
    {               /*  [204] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayPostUnixConsole__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a77u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayPostUnixConsole"
#endif
    },
    {               /*  [205] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a78u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig"
#endif
    },
    {               /*  [206] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdHshubGetMaxHshubsPerShim__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a79u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetMaxHshubsPerShim"
#endif
    },
    {               /*  [207] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpuGetHfrpInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_HFRP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuGetHfrpInfo"
#endif
    },
    {               /*  [208] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7bu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit"
#endif
    },
    {               /*  [209] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc1u)
        /*flags=*/      0xc1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask"
#endif
    },
    {               /*  [210] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl"
#endif
    },
    {               /*  [211] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits"
#endif
    },
    {               /*  [212] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a80u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo"
#endif
    },
    {               /*  [213] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfGetAuxPowerState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a81u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGetAuxPowerState"
#endif
    },
    {               /*  [214] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc1u)
        /*flags=*/      0xc1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a82u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection"
#endif
    },
    {               /*  [215] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a83u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology"
#endif
    },
    {               /*  [216] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a84u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli"
#endif
    },
    {               /*  [217] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100d0u)
        /*flags=*/      0x100d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a85u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet"
#endif
    },
    {               /*  [218] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkLinkTrainAli__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a86u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkLinkTrainAli"
#endif
    },
    {               /*  [219] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a87u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo"
#endif
    },
    {               /*  [220] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdHshubPeerConnConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a88u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubPeerConnConfig"
#endif
    },
    {               /*  [221] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdHshubGetHshubIdForLinks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetHshubIdForLinks"
#endif
    },
    {               /*  [222] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdHshubGetNumUnits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetNumUnits"
#endif
    },
    {               /*  [223] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdHshubNextHshubId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubNextHshubId"
#endif
    },
    {               /*  [224] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdHshubEgmConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubEgmConfig"
#endif
    },
    {               /*  [225] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo"
#endif
    },
    {               /*  [226] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed"
#endif
    },
    {               /*  [227] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkAreLinksTrained__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a90u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkAreLinksTrained"
#endif
    },
    {               /*  [228] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkResetLinks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a91u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkResetLinks"
#endif
    },
    {               /*  [229] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a92u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts"
#endif
    },
    {               /*  [230] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a93u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo"
#endif
    },
    {               /*  [231] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a94u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem"
#endif
    },
    {               /*  [232] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a95u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs"
#endif
    },
    {               /*  [233] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a96u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps"
#endif
    },
    {               /*  [234] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a97u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts"
#endif
    },
    {               /*  [235] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a98u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck"
#endif
    },
    {               /*  [236] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a99u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet"
#endif
    },
    {               /*  [237] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfBoostSet_2x__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*flags=*/      0x100c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostSet_2x"
#endif
    },
    {               /*  [238] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer"
#endif
    },
    {               /*  [239] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9cu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer"
#endif
    },
    {               /*  [240] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer"
#endif
    },
    {               /*  [241] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer"
#endif
    },
    {               /*  [242] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer"
#endif
    },
    {               /*  [243] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfBoostSet_3x__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*flags=*/      0x100c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostSet_3x"
#endif
    },
    {               /*  [244] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfBoostClear_3x__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*flags=*/      0x100c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostClear_3x"
#endif
    },
    {               /*  [245] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance"
#endif
    },
    {               /*  [246] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance"
#endif
    },
    {               /*  [247] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aabu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask"
#endif
    },
    {               /*  [248] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBifGetStaticInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aacu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifGetStaticInfo"
#endif
    },
    {               /*  [249] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aadu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr"
#endif
    },
    {               /*  [250] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aaeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr"
#endif
    },
    {               /*  [251] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysCleanLtcProbeFilter__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab0u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysCleanLtcProbeFilter"
#endif
    },
    {               /*  [252] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount"
#endif
    },
    {               /*  [253] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCcuGetSampleInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuGetSampleInfo"
#endif
    },
    {               /*  [254] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCcuMap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuMap"
#endif
    },
    {               /*  [255] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCcuUnmap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuUnmap"
#endif
    },
    {               /*  [256] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalSetP2pCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSetP2pCaps"
#endif
    },
    {               /*  [257] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalRemoveP2pCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRemoveP2pCaps"
#endif
    },
    {               /*  [258] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGetPcieP2pCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetPcieP2pCaps"
#endif
    },
    {               /*  [259] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBifSetPcieRo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifSetPcieRo"
#endif
    },
    {               /*  [260] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles"
#endif
    },
    {               /*  [261] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCcuSetStreamState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abdu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuSetStreamState"
#endif
    },
    {               /*  [262] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalInitGpuIntr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalInitGpuIntr"
#endif
    },
    {               /*  [263] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncOptimizeTiming__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abfu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncOptimizeTiming"
#endif
    },
    {               /*  [264] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncGetDisplayIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncGetDisplayIds"
#endif
    },
    {               /*  [265] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncSetStereoSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncSetStereoSync"
#endif
    },
    {               /*  [266] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalFbsrInit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFbsrInit"
#endif
    },
    {               /*  [267] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling"
#endif
    },
    {               /*  [268] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncGetVactiveLines__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncGetVactiveLines"
#endif
    },
    {               /*  [269] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb"
#endif
    },
    {               /*  [270] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPostInitBrightcStateLoad__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPostInitBrightcStateLoad"
#endif
    },
    {               /*  [271] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl"
#endif
    },
    {               /*  [272] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl"
#endif
    },
    {               /*  [273] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncIsDisplayIdValid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncIsDisplayIdValid"
#endif
    },
    {               /*  [274] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync"
#endif
    },
    {               /*  [275] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate"
#endif
    },
    {               /*  [276] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800accu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync"
#endif
    },
    {               /*  [277] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acdu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync"
#endif
    },
    {               /*  [278] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aceu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State"
#endif
    },
    {               /*  [279] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acfu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State"
#endif
    },
    {               /*  [280] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit"
#endif
    },
    {               /*  [281] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit"
#endif
    },
    {               /*  [282] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode"
#endif
    },
    {               /*  [283] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2"
#endif
    },
    {               /*  [284] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo"
#endif
    },
    {               /*  [285] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping"
#endif
    },
    {               /*  [286] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate"
#endif
    },
    {               /*  [287] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalFifoGetNumSecureChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoGetNumSecureChannels"
#endif
    },
    {               /*  [288] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdSpdmPartition__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSpdmPartition"
#endif
    },
    {               /*  [289] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceSpdmRetrieveTranscript__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceSpdmRetrieveTranscript"
#endif
    },
    {               /*  [290] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBifDisableSystemMemoryAccess__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifDisableSystemMemoryAccess"
#endif
    },
    {               /*  [291] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplayPinsetsToLockpins__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adcu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayPinsetsToLockpins"
#endif
    },
    {               /*  [292] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalDetectHsVideoBridge__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800addu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDetectHsVideoBridge"
#endif
    },
    {               /*  [293] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl"
#endif
    },
    {               /*  [294] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalSetStaticEdidData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adfu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSetStaticEdidData"
#endif
    },
    {               /*  [295] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys"
#endif
    },
    {               /*  [296] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeDeriveLceKeys__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeDeriveLceKeys"
#endif
    },
    {               /*  [297] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeRotateKeys__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeRotateKeys"
#endif
    },
    {               /*  [298] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation"
#endif
    },
    {               /*  [299] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeSetGpuState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeSetGpuState"
#endif
    },
    {               /*  [300] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy"
#endif
    },
    {               /*  [301] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter"
#endif
    },
    {               /*  [302] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aeau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery"
#endif
    },
    {               /*  [303] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aebu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap"
#endif
    },
    {               /*  [304] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpuSetIllum__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aecu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuSetIllum"
#endif
    },
    {               /*  [305] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aedu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar"
#endif
    },
    {               /*  [306] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpuGetPFBar1Spa__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aeeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuGetPFBar1Spa"
#endif
    },
    {               /*  [307] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af0u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated"
#endif
    },
    {               /*  [308] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalDisplayPreModeSet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af1u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDisplayPreModeSet"
#endif
    },
    {               /*  [309] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalDisplayPostModeSet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af2u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDisplayPostModeSet"
#endif
    },
    {               /*  [310] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeGetStaticInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeGetStaticInfo"
#endif
    },
    {               /*  [311] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkLfmRmAction__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkLfmRmAction"
#endif
    },
    {               /*  [312] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp"
#endif
    },
    {               /*  [313] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGetLocalAtsConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetLocalAtsConfig"
#endif
    },
    {               /*  [314] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalSetPeerAtsConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afcu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSetPeerAtsConfig"
#endif
    },
    {               /*  [315] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afdu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo"
#endif
    },
    {               /*  [316] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalInitUserSharedData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalInitUserSharedData"
#endif
    },
    {               /*  [317] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalUserSharedDataSetDataPoll__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800affu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUserSharedDataSetDataPoll"
#endif
    },
    {               /*  [318] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b01u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors"
#endif
    },
    {               /*  [319] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2"
#endif
    },
    {               /*  [320] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b05u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl"
#endif
    },
    {               /*  [321] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalConfComputeRotatePerChannelKeys__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_ROTATE_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeRotatePerChannelKeys"
#endif
    },
    {               /*  [322] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkRCUserModeChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b08u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkRCUserModeChannels"
#endif
    },
    {               /*  [323] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalUcodeInstrumentationGetState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b12u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUcodeInstrumentationGetState"
#endif
    },
    {               /*  [324] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalUcodeInstrumentationSetState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b13u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_SET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUcodeInstrumentationSetState"
#endif
    },
    {               /*  [325] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalUcodeInstrumentationGetData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b14u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUcodeInstrumentationGetData"
#endif
    },
    {               /*  [326] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalPerfClientPerfModesSetControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b20u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_CLIENT_PERF_MODES_SET_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfClientPerfModesSetControl"
#endif
    },
    {               /*  [327] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkForceDebugStallAndUpdateAmap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800b9cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_FORCE_DEBUG_STALL_AND_UPDATE_AMAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkForceDebugStallAndUpdateAmap"
#endif
    },
    {               /*  [328] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalNvlinkResumeDisableChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800badu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_RESUME_DISABLE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkResumeDisableChannels"
#endif
    },
    {               /*  [329] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdClkPmumonClkDomainsGetSamples__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801037u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CLK_PMUMON_CLK_DOMAINS_GET_SAMPLES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdClkPmumonClkDomainsGetSamples"
#endif
    },
    {               /*  [330] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdSetGpfifo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_SET_GPFIFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetGpfifo"
#endif
    },
    {               /*  [331] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetPhysicalChannelCount__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801108u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetPhysicalChannelCount"
#endif
    },
    {               /*  [332] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x30008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x30008u)
        /*flags=*/      0x30008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801109u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetInfo"
#endif
    },
    {               /*  [333] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoDisableChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannels"
#endif
    },
    {               /*  [334] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoGetChannelMemInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*flags=*/      0x10004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetChannelMemInfo"
#endif
    },
    {               /*  [335] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoGetUserdLocation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetUserdLocation"
#endif
    },
    {               /*  [336] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoObjschedSwGetLog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedSwGetLog"
#endif
    },
    {               /*  [337] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoConfigCtxswTimeout__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801110u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_CONFIG_CTXSW_TIMEOUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoConfigCtxswTimeout"
#endif
    },
    {               /*  [338] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoGetDeviceInfoTable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c040u)
        /*flags=*/      0x5c040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetDeviceInfoTable"
#endif
    },
    {               /*  [339] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoClearFaultedBit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*flags=*/      0x244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801113u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoClearFaultedBit"
#endif
    },
    {               /*  [340] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x68u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoRunlistSetSchedPolicy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x68u)
        /*flags=*/      0x68u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0x20801115u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoRunlistSetSchedPolicy"
#endif
    },
    {               /*  [341] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoUpdateChannelInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801116u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoUpdateChannelInfo"
#endif
    },
    {               /*  [342] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoDisableUsermodeChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
        /*flags=*/      0x40040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801117u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableUsermodeChannels"
#endif
    },
    {               /*  [343] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb"
#endif
    },
    {               /*  [344] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoGetAllocatedChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801119u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetAllocatedChannels"
#endif
    },
    {               /*  [345] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080111au,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannelsForKeyRotation"
#endif
    },
    {               /*  [346] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
        /*flags=*/      0x100u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080111bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2"
#endif
    },
    {               /*  [347] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoRotateKeys__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080111cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_ROTATE_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoRotateKeys"
#endif
    },
    {               /*  [348] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoObjschedGetState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedGetState"
#endif
    },
    {               /*  [349] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoObjschedSetState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801121u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedSetState"
#endif
    },
    {               /*  [350] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoObjschedGetCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedGetCaps"
#endif
    },
    {               /*  [351] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo"
#endif
    },
    {               /*  [352] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFifoQueryChannelUniqueId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoQueryChannelUniqueId"
#endif
    },
    {               /*  [353] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801201u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetInfo"
#endif
    },
    {               /*  [354] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswZcullMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801205u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswZcullMode"
#endif
    },
    {               /*  [355] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetZcullInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801206u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetZcullInfo"
#endif
    },
    {               /*  [356] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswPmMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801207u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPmMode"
#endif
    },
    {               /*  [357] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x90348u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswZcullBind__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x90348u)
        /*flags=*/      0x90348u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801208u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswZcullBind"
#endif
    },
    {               /*  [358] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswPmBind__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801209u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPmBind"
#endif
    },
    {               /*  [359] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrSetGpcTileMap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetGpcTileMap"
#endif
    },
    {               /*  [360] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswSmpcMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswSmpcMode"
#endif
    },
    {               /*  [361] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmToGpcTpcMappings"
#endif
    },
    {               /*  [362] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10348u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrSetCtxswPreemptionMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10348u)
        /*flags=*/      0x10348u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801210u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetCtxswPreemptionMode"
#endif
    },
    {               /*  [363] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswPreemptionBind__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801211u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPreemptionBind"
#endif
    },
    {               /*  [364] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrPcSamplingMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801212u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrPcSamplingMode"
#endif
    },
    {               /*  [365] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x408u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetROPInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x408u)
        /*flags=*/      0x408u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801213u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ROP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetROPInfo"
#endif
    },
    {               /*  [366] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCtxswStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801215u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxswStats"
#endif
    },
    {               /*  [367] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCtxBufferSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801218u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferSize"
#endif
    },
    {               /*  [368] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCtxBufferInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801219u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferInfo"
#endif
    },
    {               /*  [369] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetGlobalSmOrder__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGlobalSmOrder"
#endif
    },
    {               /*  [370] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCurrentResidentChannel__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCurrentResidentChannel"
#endif
    },
    {               /*  [371] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetAttributeBufferSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetAttributeBufferSize"
#endif
    },
    {               /*  [372] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGfxPoolQuerySize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolQuerySize"
#endif
    },
    {               /*  [373] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGfxPoolInitialize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801220u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolInitialize"
#endif
    },
    {               /*  [374] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGfxPoolAddSlots__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801221u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolAddSlots"
#endif
    },
    {               /*  [375] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGfxPoolRemoveSlots__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801222u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolRemoveSlots"
#endif
    },
    {               /*  [376] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCapsV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*flags=*/      0x10au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801227u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCapsV2"
#endif
    },
    {               /*  [377] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801228u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetInfoV2"
#endif
    },
    {               /*  [378] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetGpcMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGpcMask"
#endif
    },
    {               /*  [379] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetTpcMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_TPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetTpcMask"
#endif
    },
    {               /*  [380] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrSetTpcPartitionMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetTpcPartitionMode"
#endif
    },
    {               /*  [381] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetEngineContextProperties__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetEngineContextProperties"
#endif
    },
    {               /*  [382] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetSmIssueRateModifier__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801230u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmIssueRateModifier"
#endif
    },
    {               /*  [383] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrFecsBindEvtbufForUid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801231u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrFecsBindEvtbufForUid"
#endif
    },
    {               /*  [384] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetPhysGpcMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*flags=*/      0x14u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801232u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetPhysGpcMask"
#endif
    },
    {               /*  [385] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetPpcMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801233u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_PPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetPpcMask"
#endif
    },
    {               /*  [386] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetNumTpcsForGpc__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801234u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetNumTpcsForGpc"
#endif
    },
    {               /*  [387] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetCtxswModes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801235u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxswModes"
#endif
    },
    {               /*  [388] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetGpcTileMap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801236u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGpcTileMap"
#endif
    },
    {               /*  [389] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10018u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetZcullMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10018u)
        /*flags=*/      0x10018u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801237u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetZcullMask"
#endif
    },
    {               /*  [390] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*flags=*/      0x918u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801238u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2"
#endif
    },
    {               /*  [391] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801239u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo"
#endif
    },
    {               /*  [392] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrCtxswSetupBind__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080123au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswSetupBind"
#endif
    },
    {               /*  [393] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetSmIssueRateModifierV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080123cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmIssueRateModifierV2"
#endif
    },
    {               /*  [394] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080123du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl"
#endif
    },
    {               /*  [395] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrTestCtxswErrorLogs__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*flags=*/      0x100048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080123eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_TEST_CTXSW_ERROR_LOGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrTestCtxswErrorLogs"
#endif
    },
    {               /*  [396] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetInfoV2"
#endif
    },
    {               /*  [397] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetCalibrationLockFailed__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCalibrationLockFailed"
#endif
    },
    {               /*  [398] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbFlushGpuCache__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbFlushGpuCache"
#endif
    },
    {               /*  [399] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetBar1Offset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801310u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetBar1Offset"
#endif
    },
    {               /*  [400] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbIsKind__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801313u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_IS_KIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbIsKind"
#endif
    },
    {               /*  [401] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetGpuCacheInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*flags=*/      0x40148u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801315u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheInfo"
#endif
    },
    {               /*  [402] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetFBRegionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801320u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetFBRegionInfo"
#endif
    },
    {               /*  [403] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetOfflinedPages__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801322u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetOfflinedPages"
#endif
    },
    {               /*  [404] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetLTCInfoForFBP__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50158u)
        /*flags=*/      0x50158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801328u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetLTCInfoForFBP"
#endif
    },
    {               /*  [405] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGspFbStatsGet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080132au,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RM_FB_STATS_ENTRY),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspFbStatsGet"
#endif
    },
    {               /*  [406] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbStatsGet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080132fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_STATS_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbStatsGet"
#endif
    },
    {               /*  [407] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbCBCOp__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801337u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_CBC_OP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbCBCOp"
#endif
    },
    {               /*  [408] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetCtagsForCbcEviction__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801338u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCtagsForCbcEviction"
#endif
    },
    {               /*  [409] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbSetupVprRegion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetupVprRegion"
#endif
    },
    {               /*  [410] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetCliManagedOfflinedPages__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCliManagedOfflinedPages"
#endif
    },
    {               /*  [411] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCompBitCopyConstructInfo"
#endif
    },
    {               /*  [412] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbSetRrd__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_RRD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetRrd"
#endif
    },
    {               /*  [413] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbSetReadLimit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetReadLimit"
#endif
    },
    {               /*  [414] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbSetWriteLimit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801340u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetWriteLimit"
#endif
    },
    {               /*  [415] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbPatchPbrForMining__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801341u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbPatchPbrForMining"
#endif
    },
    {               /*  [416] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetMemAlignment__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801342u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetMemAlignment"
#endif
    },
    {               /*  [417] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetRemappedRows__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801344u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRemappedRows"
#endif
    },
    {               /*  [418] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetFsInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801346u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_FS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetFsInfo"
#endif
    },
    {               /*  [419] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetRowRemapperHistogram__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801347u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRowRemapperHistogram"
#endif
    },
    {               /*  [420] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetDynamicOfflinedPages__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801348u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetDynamicOfflinedPages"
#endif
    },
    {               /*  [421] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbUpdateNumaStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801350u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbUpdateNumaStatus"
#endif
    },
    {               /*  [422] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetNumaInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801351u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetNumaInfo"
#endif
    },
    {               /*  [423] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*flags=*/      0x509u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801352u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout"
#endif
    },
    {               /*  [424] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGmmuCommitTlbInvalidate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
        /*flags=*/      0x100008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801353u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGmmuCommitTlbInvalidate"
#endif
    },
    {               /*  [425] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetStaticBar1Info__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801354u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetStaticBar1Info"
#endif
    },
    {               /*  [426] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801355u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration"
#endif
    },
    {               /*  [427] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*flags=*/      0x40044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801356u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration"
#endif
    },
    {               /*  [428] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801357u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_DRAM_ENCRYPTION_INFOROM_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport"
#endif
    },
    {               /*  [429] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801358u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysQueryDramEncryptionStatus"
#endif
    },
    {               /*  [430] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x148u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetCarveoutRegionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x148u)
        /*flags=*/      0x148u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801360u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCarveoutRegionInfo"
#endif
    },
    {               /*  [431] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetRemappedBanks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801361u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_REMAPPED_BANKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRemappedBanks"
#endif
    },
    {               /*  [432] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetUgpuMemoryInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801362u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_UGPU_MEMORY_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetUgpuMemoryInfo"
#endif
    },
    {               /*  [433] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x514u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetCpuCoherentRange__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x514u)
        /*flags=*/      0x514u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801363u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CPU_COHERENT_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCpuCoherentRange"
#endif
    },
    {               /*  [434] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x554u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFbGetWprRegionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x554u)
        /*flags=*/      0x554u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801364u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_WPR_REGION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetWprRegionInfo"
#endif
    },
    {               /*  [435] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1050bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcGetArchInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1050bu)
        /*flags=*/      0x1050bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801701u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetArchInfo"
#endif
    },
    {               /*  [436] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcServiceInterrupts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801702u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcServiceInterrupts"
#endif
    },
    {               /*  [437] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcGetManufacturer__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801703u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetManufacturer"
#endif
    },
    {               /*  [438] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcChangeReplayableFaultOwnership__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcChangeReplayableFaultOwnership"
#endif
    },
    {               /*  [439] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcGetEngineNotificationIntrVectors__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170du,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetEngineNotificationIntrVectors"
#endif
    },
    {               /*  [440] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcGetStaticIntrTable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetStaticIntrTable"
#endif
    },
    {               /*  [441] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdMcGetIntrCategorySubtreeMap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_INTR_CATEGORY_SUBTREE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetIntrCategorySubtreeMap"
#endif
    },
    {               /*  [442] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10518u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPciInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10518u)
        /*flags=*/      0x10518u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciInfo"
#endif
    },
    {               /*  [443] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10518u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPciBarInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10518u)
        /*flags=*/      0x10518u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801803u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciBarInfo"
#endif
    },
    {               /*  [444] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetPcieLinkWidth__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801804u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieLinkWidth"
#endif
    },
    {               /*  [445] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetPcieSpeed__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801805u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieSpeed"
#endif
    },
    {               /*  [446] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusServiceGpuMultifunctionState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801812u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusServiceGpuMultifunctionState"
#endif
    },
    {               /*  [447] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPexCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801813u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexCounters"
#endif
    },
    {               /*  [448] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusClearPexCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801814u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusClearPexCounters"
#endif
    },
    {               /*  [449] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusFreezePexCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801815u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusFreezePexCounters"
#endif
    },
    {               /*  [450] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPexLaneCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801816u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexLaneCounters"
#endif
    },
    {               /*  [451] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPcieLtrLatency__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801817u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieLtrLatency"
#endif
    },
    {               /*  [452] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetPcieLtrLatency__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801818u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieLtrLatency"
#endif
    },
    {               /*  [453] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPexUtilCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801819u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexUtilCounters"
#endif
    },
    {               /*  [454] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusClearPexUtilCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801820u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusClearPexUtilCounters"
#endif
    },
    {               /*  [455] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801823u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetInfoV2"
#endif
    },
    {               /*  [456] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusControlPublicAspmBits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801824u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusControlPublicAspmBits"
#endif
    },
    {               /*  [457] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetNvlinkPeerIdMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801825u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkPeerIdMask"
#endif
    },
    {               /*  [458] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetEomParameters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801826u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetEomParameters"
#endif
    },
    {               /*  [459] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetUphyDlnCfgSpace__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801827u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetUphyDlnCfgSpace"
#endif
    },
    {               /*  [460] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetEomStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801828u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetEomStatus"
#endif
    },
    {               /*  [461] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801829u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieReqAtomicsCaps"
#endif
    },
    {               /*  [462] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182au,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics"
#endif
    },
    {               /*  [463] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetC2CInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetC2CInfo"
#endif
    },
    {               /*  [464] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSysmemAccess__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000u)
        /*flags=*/      0x1000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSysmemAccess"
#endif
    },
    {               /*  [465] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetC2CErrorInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182du,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_C2C_ERR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetC2CErrorInfo"
#endif
    },
    {               /*  [466] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetP2pMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*flags=*/      0x50040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetP2pMapping"
#endif
    },
    {               /*  [467] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusUnsetP2pMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*flags=*/      0x50040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusUnsetP2pMapping"
#endif
    },
    {               /*  [468] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*flags=*/      0x40448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801830u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieCplAtomicsCaps"
#endif
    },
    {               /*  [469] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetC2CLpwrStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801831u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetC2CLpwrStats"
#endif
    },
    {               /*  [470] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetC2CLpwrStateVote__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801832u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetC2CLpwrStateVote"
#endif
    },
    {               /*  [471] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusSetC2CIdleThreshold__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801836u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_IDLE_THRESHOLD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetC2CIdleThreshold"
#endif
    },
    {               /*  [472] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetC2CPacketCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801837u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_C2C_PACKET_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetC2CPacketCounters"
#endif
    },
    {               /*  [473] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10518u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetPciInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10518u)
        /*flags=*/      0x10518u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080183cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciInfoV2"
#endif
    },
    {               /*  [474] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10318u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdKPerfBoost__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10318u)
        /*flags=*/      0x10318u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080200au,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_BOOST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKPerfBoost"
#endif
    },
    {               /*  [475] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfGetLevelInfo_V2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080200bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetLevelInfo_V2"
#endif
    },
    {               /*  [476] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfGetPowerstate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080205au,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetPowerstate"
#endif
    },
    {               /*  [477] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfSetPowerstate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080205bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfSetPowerstate"
#endif
    },
    {               /*  [478] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfNotifyVideoevent__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080205du,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfNotifyVideoevent"
#endif
    },
    {               /*  [479] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfGetCurrentPstate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802068u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetCurrentPstate"
#endif
    },
    {               /*  [480] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfGetTegraPerfmonSample__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802069u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_TEGRA_PERFMON_SAMPLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetTegraPerfmonSample"
#endif
    },
    {               /*  [481] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfRatedTdpGetControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*flags=*/      0x4au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080206eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfRatedTdpGetControl"
#endif
    },
    {               /*  [482] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfRatedTdpSetControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080206fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfRatedTdpSetControl"
#endif
    },
    {               /*  [483] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802087u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample"
#endif
    },
    {               /*  [484] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfSetAuxPowerState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802092u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfSetAuxPowerState"
#endif
    },
    {               /*  [485] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfReservePerfmonHw__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802093u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfReservePerfmonHw"
#endif
    },
    {               /*  [486] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50008u)
        /*flags=*/      0x50008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802096u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2"
#endif
    },
    {               /*  [487] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcReadVirtualMem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802204u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcReadVirtualMem"
#endif
    },
    {               /*  [488] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcGetErrorCount__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802205u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetErrorCount"
#endif
    },
    {               /*  [489] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcSetCleanErrorHistory__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802207u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcSetCleanErrorHistory"
#endif
    },
    {               /*  [490] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcGetWatchdogInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802209u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetWatchdogInfo"
#endif
    },
    {               /*  [491] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcDisableWatchdog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220au,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcDisableWatchdog"
#endif
    },
    {               /*  [492] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcEnableWatchdog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220bu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcEnableWatchdog"
#endif
    },
    {               /*  [493] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcReleaseWatchdogRequests__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220cu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcReleaseWatchdogRequests"
#endif
    },
    {               /*  [494] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdSetRcRecovery__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*flags=*/      0x40154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcRecovery"
#endif
    },
    {               /*  [495] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetRcRecovery__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*flags=*/      0x40154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetRcRecovery"
#endif
    },
    {               /*  [496] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcSoftDisableWatchdog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802210u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcSoftDisableWatchdog"
#endif
    },
    {               /*  [497] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdSetRcInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802211u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcInfo"
#endif
    },
    {               /*  [498] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetRcInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802212u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetRcInfo"
#endif
    },
    {               /*  [499] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcGetErrorV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802213u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_ERROR_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetErrorV2"
#endif
    },
    {               /*  [500] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdSetRcWatchdogInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802214u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_WATCHDOG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcWatchdogInfo"
#endif
    },
    {               /*  [501] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdRcPauseWatchdog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*flags=*/      0x14u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802216u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcPauseWatchdog"
#endif
    },
    {               /*  [502] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpioProgramDirection__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802300u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioProgramDirection"
#endif
    },
    {               /*  [503] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpioProgramOutput__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802301u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioProgramOutput"
#endif
    },
    {               /*  [504] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpioReadInput__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802302u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioReadInput"
#endif
    },
    {               /*  [505] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGpioActivateHwFunction__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioActivateHwFunction"
#endif
    },
    {               /*  [506] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvdGetDumpSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetDumpSize"
#endif
    },
    {               /*  [507] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvdGetDump__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802402u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetDump"
#endif
    },
    {               /*  [508] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvdGetNocatJournalRpt__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802409u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetNocatJournalRpt"
#endif
    },
    {               /*  [509] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvdSetNocatJournalData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080240bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdSetNocatJournalData"
#endif
    },
    {               /*  [510] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvdInsertNocatJournalRecord__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080240cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdInsertNocatJournalRecord"
#endif
    },
    {               /*  [511] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDmaInvalidateTLB__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802502u,
        /*paramSize=*/  sizeof(NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDmaInvalidateTLB"
#endif
    },
    {               /*  [512] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdDmaGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802503u,
        /*paramSize=*/  sizeof(NV2080_CTRL_DMA_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDmaGetInfo"
#endif
    },
    {               /*  [513] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdPmgrGetModuleInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802609u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PMGR_MODULE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPmgrGetModuleInfo"
#endif
    },
    {               /*  [514] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGc6Entry__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*flags=*/      0x2u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080270du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GC6_ENTRY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGc6Entry"
#endif
    },
    {               /*  [515] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGc6Exit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*flags=*/      0x2u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080270eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GC6_EXIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGc6Exit"
#endif
    },
    {               /*  [516] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdLpwrDifrCtrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdLpwrDifrCtrl"
#endif
    },
    {               /*  [517] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdLpwrDifrPrefetchResponse__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802802u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdLpwrDifrPrefetchResponse"
#endif
    },
    {               /*  [518] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCaps"
#endif
    },
    {               /*  [519] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x30349u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetCePceMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x30349u)
        /*flags=*/      0x30349u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a02u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCePceMask"
#endif
    },
    {               /*  [520] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetCapsV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCapsV2"
#endif
    },
    {               /*  [521] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeUpdatePceLceMappings__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a05u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdatePceLceMappings"
#endif
    },
    {               /*  [522] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeUpdateClassDB__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a06u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdateClassDB"
#endif
    },
    {               /*  [523] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetPhysicalCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
        /*flags=*/      0x101d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetPhysicalCaps"
#endif
    },
    {               /*  [524] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetFaultMethodBufferSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c040u)
        /*flags=*/      0x1c040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a08u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetFaultMethodBufferSize"
#endif
    },
    {               /*  [525] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetHubPceMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*flags=*/      0x4c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a09u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetHubPceMask"
#endif
    },
    {               /*  [526] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetAllCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0au,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetAllCaps"
#endif
    },
    {               /*  [527] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetAllPhysicalCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
        /*flags=*/      0x101d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetAllPhysicalCaps"
#endif
    },
    {               /*  [528] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x149u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetLceShimInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x149u)
        /*flags=*/      0x149u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetLceShimInfo"
#endif
    },
    {               /*  [529] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeUpdatePceLceMappingsV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdatePceLceMappingsV2"
#endif
    },
    {               /*  [530] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetHubPceMaskV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetHubPceMaskV2"
#endif
    },
    {               /*  [531] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetPceConfigForLceType__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetPceConfigForLceType"
#endif
    },
    {               /*  [532] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetDecompLceMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
        /*flags=*/      0x154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a11u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetDecompLceMask"
#endif
    },
    {               /*  [533] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeIsDecompLceEnabled__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
        /*flags=*/      0x154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a12u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeIsDecompLceEnabled"
#endif
    },
    {               /*  [534] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x102c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeGetPceConfigForLceMIGGpuInstance__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x102c0u)
        /*flags=*/      0x102c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a13u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_MIG_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetPceConfigForLceMIGGpuInstance"
#endif
    },
    {               /*  [535] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdCeApplyPceLceMappingForMIGGpuInstance__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a14u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_PCE_LCE_MIG_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeApplyPceLceMappingForMIGGpuInstance"
#endif
    },
    {               /*  [536] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetNvlinkCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803001u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkCaps"
#endif
    },
    {               /*  [537] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetNvlinkStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803002u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkStatus"
#endif
    },
    {               /*  [538] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBusGetNvlinkErrInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803003u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkErrInfo"
#endif
    },
    {               /*  [539] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetNvlinkCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803004u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkCounters"
#endif
    },
    {               /*  [540] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdClearNvlinkCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803005u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdClearNvlinkCounters"
#endif
    },
    {               /*  [541] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803009u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts"
#endif
    },
    {               /*  [542] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetupEom__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080300cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetupEom"
#endif
    },
    {               /*  [543] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetPowerState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080300eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetPowerState"
#endif
    },
    {               /*  [544] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinGetLinkFomValues__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803011u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinGetLinkFomValues"
#endif
    },
    {               /*  [545] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetNvlinkEccErrors__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803014u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetNvlinkEccErrors"
#endif
    },
    {               /*  [546] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkReadTpCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803015u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkReadTpCounters"
#endif
    },
    {               /*  [547] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetLpCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803018u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLpCounters"
#endif
    },
    {               /*  [548] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetLoopbackMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803023u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetLoopbackMode"
#endif
    },
    {               /*  [549] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetRefreshCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803028u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetRefreshCounters"
#endif
    },
    {               /*  [550] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkClearRefreshCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803029u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkClearRefreshCounters"
#endif
    },
    {               /*  [551] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803038u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr"
#endif
    },
    {               /*  [552] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
        /*flags=*/      0x10041u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803039u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo"
#endif
    },
    {               /*  [553] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkEnableLinks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303au,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableLinks"
#endif
    },
    {               /*  [554] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProcessInitDisabledLinks"
#endif
    },
    {               /*  [555] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkEomControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEomControl"
#endif
    },
    {               /*  [556] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetL1Threshold__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetL1Threshold"
#endif
    },
    {               /*  [557] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetL1Threshold__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetL1Threshold"
#endif
    },
    {               /*  [558] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10250u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkInbandSendData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10250u)
        /*flags=*/      0x10250u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803040u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkInbandSendData"
#endif
    },
    {               /*  [559] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkIsGpuDegraded__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803041u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkIsGpuDegraded"
#endif
    },
    {               /*  [560] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkDirectConnectCheck__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803042u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkDirectConnectCheck"
#endif
    },
    {               /*  [561] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkPostFaultUp__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803043u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPostFaultUp"
#endif
    },
    {               /*  [562] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetPortEvents__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803044u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetPortEvents"
#endif
    },
    {               /*  [563] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkCycleLink__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803045u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CYCLE_LINK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkCycleLink"
#endif
    },
    {               /*  [564] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdIsNvlinkReducedConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803046u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIsNvlinkReducedConfig"
#endif
    },
    {               /*  [565] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetNvlinkCountersV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803050u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkCountersV2"
#endif
    },
    {               /*  [566] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdClearNvlinkCountersV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803051u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdClearNvlinkCountersV2"
#endif
    },
    {               /*  [567] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkClearLpCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803052u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkClearLpCounters"
#endif
    },
    {               /*  [568] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetNvlinkSupportedCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803074u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkSupportedCounters"
#endif
    },
    {               /*  [569] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803081u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings"
#endif
    },
    {               /*  [570] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803082u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings"
#endif
    },
    {               /*  [571] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetPlatformInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803083u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetPlatformInfo"
#endif
    },
    {               /*  [572] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetSupportedBWMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803085u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetSupportedBWMode"
#endif
    },
    {               /*  [573] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetBWMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803086u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetBWMode"
#endif
    },
    {               /*  [574] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetBWMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803087u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetBWMode"
#endif
    },
    {               /*  [575] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetLocalDeviceInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803088u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLocalDeviceInfo"
#endif
    },
    {               /*  [576] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkInjectSWError__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803089u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkInjectSWError"
#endif
    },
    {               /*  [577] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkPostLazyErrorRecovery__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080308au,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPostLazyErrorRecovery"
#endif
    },
    {               /*  [578] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080308bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo"
#endif
    },
    {               /*  [579] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkUpdateNvleTopology__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080308cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateNvleTopology"
#endif
    },
    {               /*  [580] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetUpdateNvleLids__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080308du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetUpdateNvleLids"
#endif
    },
    {               /*  [581] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkConfigureL1Toggle__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080308eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkConfigureL1Toggle"
#endif
    },
    {               /*  [582] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlNvlinkGetL1Toggle__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080308fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlNvlinkGetL1Toggle"
#endif
    },
    {               /*  [583] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803091u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo"
#endif
    },
    {               /*  [584] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetNvleEnabledState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803092u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetNvleEnabledState"
#endif
    },
    {               /*  [585] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkPRMAccess__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803095u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccess"
#endif
    },
    {               /*  [586] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetLinkAccumulativeMetricData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803097u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkAccumulativeMetricData"
#endif
    },
    {               /*  [587] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetLinkRecordMetricData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803098u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_RECORD_METRIC_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkRecordMetricData"
#endif
    },
    {               /*  [588] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetDeviceRecordMetricData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803099u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_DEVICE_RECORD_METRIC_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetDeviceRecordMetricData"
#endif
    },
    {               /*  [589] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080309au,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL"
#endif
    },
    {               /*  [590] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080309bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL"
#endif
    },
    {               /*  [591] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkUpdateClid__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080309cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_CLID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateClid"
#endif
    },
    {               /*  [592] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkLockRemapTableAndMse__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*flags=*/      0x54u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080309du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_LOCK_REMAP_TABLE_AND_MSE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkLockRemapTableAndMse"
#endif
    },
    {               /*  [593] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetRemapTableInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*flags=*/      0x54u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080309eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_REMAP_TABLE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetRemapTableInfo"
#endif
    },
    {               /*  [594] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetNvlePktCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*flags=*/      0x54u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080309fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_NVLE_PKT_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetNvlePktCounters"
#endif
    },
    {               /*  [595] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetupNvleEncryptionKey__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208030a0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SETUP_NVLE_ENCRYPTION_KEY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetupNvleEncryptionKey"
#endif
    },
    {               /*  [596] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkSetBWModeAsync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208030a1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_BW_MODE_ASYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetBWModeAsync"
#endif
    },
    {               /*  [597] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetRemapTableInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x54u)
        /*flags=*/      0x54u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208030a2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_REMAP_TABLE_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetRemapTableInfoV2"
#endif
    },
    {               /*  [598] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetUpdateNvleLidsV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208030a3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetUpdateNvleLidsV2"
#endif
    },
    {               /*  [599] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdNvlinkGetLinkTrainingTime__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208030a4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_TRAINING_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkTrainingTime"
#endif
    },
    {               /*  [600] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnGetDmemUsage__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803101u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetDmemUsage"
#endif
    },
    {               /*  [601] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnGetEngineArch__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetEngineArch"
#endif
    },
    {               /*  [602] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnUstreamerQueueInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerQueueInfo"
#endif
    },
    {               /*  [603] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnUstreamerControlGet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerControlGet"
#endif
    },
    {               /*  [604] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnUstreamerControlSet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerControlSet"
#endif
    },
    {               /*  [605] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnGetCtxBufferInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetCtxBufferInfo"
#endif
    },
    {               /*  [606] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlcnGetCtxBufferSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803125u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetCtxBufferSize"
#endif
    },
    {               /*  [607] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccGetClientExposedCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803400u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetClientExposedCounters"
#endif
    },
    {               /*  [608] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccGetVolatileCounts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetVolatileCounts"
#endif
    },
    {               /*  [609] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccGetSramUniqueUncorrCounts__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803402u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetSramUniqueUncorrCounts"
#endif
    },
    {               /*  [610] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccInjectError__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803403u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_INJECT_ERROR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccInjectError"
#endif
    },
    {               /*  [611] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccGetRepairStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803404u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetRepairStatus"
#endif
    },
    {               /*  [612] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccInjectionSupported__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803405u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_INJECTION_SUPPORTED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccInjectionSupported"
#endif
    },
    {               /*  [613] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdEccGetUnrepairableMemoryFlag__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803406u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_UNREPAIRABLE_MEMORY_FLAG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetUnrepairableMemoryFlag"
#endif
    },
    {               /*  [614] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlaRange__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803501u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaRange"
#endif
    },
    {               /*  [615] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlaSetupInstanceMemBlock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*flags=*/      0x10244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803502u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaSetupInstanceMemBlock"
#endif
    },
    {               /*  [616] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlaGetRange__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*flags=*/      0x10004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803503u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_GET_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaGetRange"
#endif
    },
    {               /*  [617] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdFlaGetFabricMemStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10118u)
        /*flags=*/      0x10118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803504u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaGetFabricMemStats"
#endif
    },
    {               /*  [618] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40549u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGspGetFeatures__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40549u)
        /*flags=*/      0x40549u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803601u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GSP_GET_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspGetFeatures"
#endif
    },
    {               /*  [619] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGspGetRmHeapStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803602u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspGetRmHeapStats"
#endif
    },
    {               /*  [620] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetVgpuHeapStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803603u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVgpuHeapStats"
#endif
    },
    {               /*  [621] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdLibosGetHeapStats__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803604u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdLibosGetHeapStats"
#endif
    },
    {               /*  [622] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGspGdmaFuzzTest__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100004u)
        /*flags=*/      0x100004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803605u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GSP_GDMA_FUZZ_TEST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspGdmaFuzzTest"
#endif
    },
    {               /*  [623] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGspCoreTest__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100044u)
        /*flags=*/      0x100044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803606u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GSP_CORE_TEST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspCoreTest"
#endif
    },
    {               /*  [624] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGrmgrGetGrFsInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrmgrGetGrFsInfo"
#endif
    },
    {               /*  [625] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdBifGetData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803b01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIF_GET_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifGetData"
#endif
    },
    {               /*  [626] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*flags=*/      0x3u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixGc6BlockerRefCnt"
#endif
    },
    {               /*  [627] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdOsUnixAllowDisallowGcoff__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d02u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixAllowDisallowGcoff"
#endif
    },
    {               /*  [628] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdOsUnixAudioDynamicPower__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*flags=*/      0x1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixAudioDynamicPower"
#endif
    },
    {               /*  [629] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xbu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xbu)
        /*flags=*/      0xbu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixVidmemPersistenceStatus"
#endif
    },
    {               /*  [630] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdOsUnixUpdateTgpStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d08u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixUpdateTgpStatus"
#endif
    },
    {               /*  [631] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804001u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask"
#endif
    },
    {               /*  [632] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804002u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask"
#endif
    },
    {               /*  [633] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804003u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType"
#endif
    },
    {               /*  [634] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804004u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu"
#endif
    },
    {               /*  [635] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804005u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo"
#endif
    },
    {               /*  [636] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804006u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage"
#endif
    },
    {               /*  [637] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*flags=*/      0x1d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804007u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity"
#endif
    },
    {               /*  [638] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804008u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources"
#endif
    },
    {               /*  [639] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804009u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding"
#endif
    },
    {               /*  [640] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400au,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport"
#endif
    },
    {               /*  [641] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig"
#endif
    },
    {               /*  [642] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalFreeStates__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalFreeStates"
#endif
    },
    {               /*  [643] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400du,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_GET_FRAME_RATE_LIMITER_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus"
#endif
    },
    {               /*  [644] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_HETEROGENEOUS_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode"
#endif
    },
    {               /*  [645] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode"
#endif
    },
    {               /*  [646] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalScrubGuestFb__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804011u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SCRUB_GUEST_FB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalScrubGuestFb"
#endif
    },
    {               /*  [647] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalCheckScrubComplete__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804012u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_CHECK_SCRUB_COMPLETE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalCheckScrubComplete"
#endif
    },
    {               /*  [648] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalAllocGspCeUtils__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804013u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_ALLOC_GSP_CEUTILS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalAllocGspCeUtils"
#endif
    },
    {               /*  [649] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdVgpuMgrInternalFreeGspCeUtils__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804014u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_GSP_CEUTILS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalFreeGspCeUtils"
#endif
    },
    {               /*  [650] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGetAvailableHshubMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804101u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetAvailableHshubMask"
#endif
    },
    {               /*  [651] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlSetEcThrottleMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlSetEcThrottleMode"
#endif
    },
    {               /*  [652] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuIsResetCoupled__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208081f3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_IS_RESET_COUPLED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuIsResetCoupled"
#endif
    },
    {               /*  [653] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetDieletInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208081f4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_DIELET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetDieletInfo"
#endif
    },
    {               /*  [654] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdGpuGetUnitFsInfoFromChiplet__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208081f5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_UNIT_FS_INFO_FROM_DIELET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetUnitFsInfoFromChiplet"
#endif
    },
    {               /*  [655] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &subdeviceCtrlCmdInternalGcxEntryPrerequisite__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080a7d7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGcxEntryPrerequisite"
#endif
    },
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__Subdevice __nvoc_metadata__Subdevice = {
    .rtti.pClassDef = &__nvoc_class_def_Subdevice,    // (subdevice) this
    .rtti.dtor      = &__nvoc_dtor_Subdevice,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(Subdevice, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__subdevicePreDestruct__ = &subdevicePreDestruct_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &__nvoc_down_thunk_Subdevice_resPreDestruct,    // virtual
    .vtable.__subdeviceInternalControlForward__ = &subdeviceInternalControlForward_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &__nvoc_down_thunk_Subdevice_gpuresInternalControlForward,    // virtual
    .vtable.__subdeviceControl__ = &__nvoc_up_thunk_GpuResource_subdeviceControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__subdeviceMap__ = &__nvoc_up_thunk_GpuResource_subdeviceMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__subdeviceUnmap__ = &__nvoc_up_thunk_GpuResource_subdeviceUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__subdeviceShareCallback__ = &__nvoc_up_thunk_GpuResource_subdeviceShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__subdeviceGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__subdeviceGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_subdeviceGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__subdeviceGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_subdeviceGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__subdeviceAccessCallback__ = &__nvoc_up_thunk_RmResource_subdeviceAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__subdeviceGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_subdeviceGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__subdeviceCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_subdeviceCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__subdeviceGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_subdeviceGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__subdeviceControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_subdeviceControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__subdeviceControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_subdeviceControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__subdeviceControl_Prologue__ = &__nvoc_up_thunk_RmResource_subdeviceControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__subdeviceControl_Epilogue__ = &__nvoc_up_thunk_RmResource_subdeviceControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__subdeviceCanCopy__ = &__nvoc_up_thunk_RsResource_subdeviceCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__subdeviceIsDuplicate__ = &__nvoc_up_thunk_RsResource_subdeviceIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__subdeviceControlFilter__ = &__nvoc_up_thunk_RsResource_subdeviceControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__subdeviceIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_subdeviceIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__subdeviceMapTo__ = &__nvoc_up_thunk_RsResource_subdeviceMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__subdeviceUnmapFrom__ = &__nvoc_up_thunk_RsResource_subdeviceUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__subdeviceGetRefCount__ = &__nvoc_up_thunk_RsResource_subdeviceGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__subdeviceAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_subdeviceAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__subdeviceGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_subdeviceGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__subdeviceGetNotificationShare__ = &__nvoc_up_thunk_Notifier_subdeviceGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__subdeviceSetNotificationShare__ = &__nvoc_up_thunk_Notifier_subdeviceSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__subdeviceUnregisterEvent__ = &__nvoc_up_thunk_Notifier_subdeviceUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__subdeviceGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_subdeviceGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Subdevice = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__Subdevice.rtti,    // [0]: (subdevice) this
        &__nvoc_metadata__Subdevice.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__Subdevice.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__Subdevice.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in Subdevice from superclasses

// subdevicePreDestruct: virtual override (res) base (gpures)
void __nvoc_down_thunk_Subdevice_resPreDestruct(struct RsResource *pResource) {
    subdevicePreDestruct((struct Subdevice *)(((unsigned char *) pResource) - NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// subdeviceInternalControlForward: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_Subdevice_gpuresInternalControlForward(struct GpuResource *pSubdevice, NvU32 command, void *pParams, NvU32 size) {
    return subdeviceInternalControlForward((struct Subdevice *)(((unsigned char *) pSubdevice) - NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), command, pParams, size);
}


// 28 up-thunk(s) defined to bridge methods in Subdevice to superclasses

// subdeviceControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceControl(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// subdeviceMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceMap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// subdeviceUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceUnmap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// subdeviceShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_subdeviceShareCallback(struct Subdevice *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// subdeviceGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize(struct Subdevice *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// subdeviceGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceGetMapAddrSpace(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// subdeviceGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_subdeviceGetInternalObjectHandle(struct Subdevice *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource)));
}

// subdeviceAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_subdeviceAccessCallback(struct Subdevice *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// subdeviceGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceGetMemInterMapParams(struct Subdevice *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// subdeviceCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceCheckMemInterUnmap(struct Subdevice *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// subdeviceGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceGetMemoryMappingDescriptor(struct Subdevice *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// subdeviceControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceControlSerialization_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// subdeviceControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_subdeviceControlSerialization_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// subdeviceControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_subdeviceControl_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// subdeviceControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_subdeviceControl_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// subdeviceCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_subdeviceCanCopy(struct Subdevice *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// subdeviceIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceIsDuplicate(struct Subdevice *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// subdeviceControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceControlFilter(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// subdeviceIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_subdeviceIsPartialUnmapSupported(struct Subdevice *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// subdeviceMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceMapTo(struct Subdevice *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// subdeviceUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_subdeviceUnmapFrom(struct Subdevice *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// subdeviceGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_subdeviceGetRefCount(struct Subdevice *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// subdeviceAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_subdeviceAddAdditionalDependants(struct RsClient *pClient, struct Subdevice *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// subdeviceGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_subdeviceGetNotificationListPtr(struct Subdevice *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(Subdevice, __nvoc_base_Notifier)));
}

// subdeviceGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_subdeviceGetNotificationShare(struct Subdevice *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(Subdevice, __nvoc_base_Notifier)));
}

// subdeviceSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_subdeviceSetNotificationShare(struct Subdevice *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(Subdevice, __nvoc_base_Notifier)), pNotifShare);
}

// subdeviceUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_subdeviceUnregisterEvent(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(Subdevice, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// subdeviceGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_subdeviceGetOrAllocNotifShare(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(Subdevice, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__Subdevice = 
{
    .numEntries=     656,
    .pExportEntries= __nvoc_exported_method_def_Subdevice
};


// Destruct Subdevice object.
void __nvoc_subdeviceDestruct(Subdevice*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_Subdevice(Dynamic* pThis) {

    Subdevice *__nvoc_this = (Subdevice *) pThis;

// Call destructor.
    __nvoc_subdeviceDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}


// Construct Subdevice object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_Subdevice(Subdevice *pResource, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pResource->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pResource->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_Subdevice(pResource, pRmhalspecowner, pGpuhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_subdeviceConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail__init;
    goto __nvoc_ctor_Subdevice_exit; // Success

    // Unwind on error.
__nvoc_ctor_Subdevice_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pResource->__nvoc_base_Notifier);
__nvoc_ctor_Subdevice_fail_Notifier:
    __nvoc_dtor_GpuResource((Dynamic *)&pResource->__nvoc_base_GpuResource);
__nvoc_ctor_Subdevice_fail_GpuResource:
__nvoc_ctor_Subdevice_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Subdevice_1(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // subdeviceCtrlCmdBiosGetInfoV2 -- halified (2 hals) exported (id=0x20800810) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBiosGetInfoV2__ = &subdeviceCtrlCmdBiosGetInfoV2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBiosGetInfoV2__ = &subdeviceCtrlCmdBiosGetInfoV2_92bfc3;
    }

    // subdeviceCtrlCmdBiosGetSKUInfo -- halified (2 hals) exported (id=0x20800808)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBiosGetSKUInfo__ = &subdeviceCtrlCmdBiosGetSKUInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBiosGetSKUInfo__ = &subdeviceCtrlCmdBiosGetSKUInfo_KERNEL;
    }

    // subdeviceCtrlCmdBiosGetPostTime -- halified (2 hals) exported (id=0x20800809) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBiosGetPostTime__ = &subdeviceCtrlCmdBiosGetPostTime_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBiosGetPostTime__ = &subdeviceCtrlCmdBiosGetPostTime_92bfc3;
    }

    // subdeviceCtrlCmdClkPmumonClkDomainsGetSamples -- halified (2 hals) exported (id=0x20801037) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdClkPmumonClkDomainsGetSamples__ = &subdeviceCtrlCmdClkPmumonClkDomainsGetSamples_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdClkPmumonClkDomainsGetSamples__ = &subdeviceCtrlCmdClkPmumonClkDomainsGetSamples_92bfc3;
    }

    // subdeviceCtrlCmdBusGetPcieReqAtomicsCaps -- halified (2 hals) exported (id=0x20801829) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_92bfc3;
    }

    // subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics -- halified (2 hals) exported (id=0x2080182a) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__ = &subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__ = &subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_92bfc3;
    }

    // subdeviceCtrlCmdBusGetPcieCplAtomicsCaps -- halified (2 hals) exported (id=0x20801830) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_92bfc3;
    }

    // subdeviceCtrlCmdBusGetC2CInfo -- halified (2 hals) exported (id=0x2080182b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetC2CInfo__ = &subdeviceCtrlCmdBusGetC2CInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetC2CInfo__ = &subdeviceCtrlCmdBusGetC2CInfo_92bfc3;
    }

    // subdeviceCtrlCmdBusGetC2CLpwrStats -- halified (2 hals) exported (id=0x20801831) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetC2CLpwrStats__ = &subdeviceCtrlCmdBusGetC2CLpwrStats_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetC2CLpwrStats__ = &subdeviceCtrlCmdBusGetC2CLpwrStats_92bfc3;
    }

    // subdeviceCtrlCmdBusSetC2CLpwrStateVote -- halified (2 hals) exported (id=0x20801832) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusSetC2CLpwrStateVote__ = &subdeviceCtrlCmdBusSetC2CLpwrStateVote_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusSetC2CLpwrStateVote__ = &subdeviceCtrlCmdBusSetC2CLpwrStateVote_92bfc3;
    }

    // subdeviceCtrlCmdBusSetC2CIdleThreshold -- halified (2 hals) exported (id=0x20801836) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusSetC2CIdleThreshold__ = &subdeviceCtrlCmdBusSetC2CIdleThreshold_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusSetC2CIdleThreshold__ = &subdeviceCtrlCmdBusSetC2CIdleThreshold_92bfc3;
    }

    // subdeviceCtrlCmdBusSetP2pMapping -- halified (2 hals) exported (id=0x2080182e) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusSetP2pMapping__ = &subdeviceCtrlCmdBusSetP2pMapping_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusSetP2pMapping__ = &subdeviceCtrlCmdBusSetP2pMapping_5baef9;
    }

    // subdeviceCtrlCmdBusUnsetP2pMapping -- halified (2 hals) exported (id=0x2080182f) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusUnsetP2pMapping__ = &subdeviceCtrlCmdBusUnsetP2pMapping_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusUnsetP2pMapping__ = &subdeviceCtrlCmdBusUnsetP2pMapping_5baef9;
    }

    // subdeviceCtrlCmdBusGetNvlinkCaps -- halified (2 hals) exported (id=0x20803001)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetNvlinkCaps__ = &subdeviceCtrlCmdBusGetNvlinkCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetNvlinkCaps__ = &subdeviceCtrlCmdBusGetNvlinkCaps_IMPL;
    }

    // subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2 -- halified (2 hals) exported (id=0x20802096)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__ = &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__ = &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_KERNEL;
    }

    // subdeviceCtrlCmdPerfReservePerfmonHw -- halified (2 hals) exported (id=0x20802093) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfReservePerfmonHw__ = &subdeviceCtrlCmdPerfReservePerfmonHw_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfReservePerfmonHw__ = &subdeviceCtrlCmdPerfReservePerfmonHw_KERNEL;
    }

    // subdeviceCtrlCmdPerfGetLevelInfo_V2 -- halified (2 hals) exported (id=0x2080200b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetLevelInfo_V2__ = &subdeviceCtrlCmdPerfGetLevelInfo_V2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetLevelInfo_V2__ = &subdeviceCtrlCmdPerfGetLevelInfo_V2_92bfc3;
    }

    // subdeviceCtrlCmdPerfGetCurrentPstate -- halified (2 hals) exported (id=0x20802068) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetCurrentPstate__ = &subdeviceCtrlCmdPerfGetCurrentPstate_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetCurrentPstate__ = &subdeviceCtrlCmdPerfGetCurrentPstate_92bfc3;
    }

    // subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample -- halified (2 hals) exported (id=0x20802087) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__ = &subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__ = &subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_92bfc3;
    }

    // subdeviceCtrlCmdPerfGetPowerstate -- halified (2 hals) exported (id=0x2080205a) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetPowerstate__ = &subdeviceCtrlCmdPerfGetPowerstate_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetPowerstate__ = &subdeviceCtrlCmdPerfGetPowerstate_92bfc3;
    }

    // subdeviceCtrlCmdPerfNotifyVideoevent -- halified (2 hals) exported (id=0x2080205d) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfNotifyVideoevent__ = &subdeviceCtrlCmdPerfNotifyVideoevent_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfNotifyVideoevent__ = &subdeviceCtrlCmdPerfNotifyVideoevent_92bfc3;
    }

    // subdeviceCtrlCmdFbGetOfflinedPages -- halified (2 hals) exported (id=0x20801322) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFbGetOfflinedPages__ = &subdeviceCtrlCmdFbGetOfflinedPages_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFbGetOfflinedPages__ = &subdeviceCtrlCmdFbGetOfflinedPages_92bfc3;
    }

    // subdeviceCtrlCmdFbGetLTCInfoForFBP -- halified (2 hals) exported (id=0x20801328) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFbGetLTCInfoForFBP__ = &subdeviceCtrlCmdFbGetLTCInfoForFBP_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFbGetLTCInfoForFBP__ = &subdeviceCtrlCmdFbGetLTCInfoForFBP_92bfc3;
    }

    // subdeviceCtrlCmdFbGetDynamicOfflinedPages -- halified (2 hals) exported (id=0x20801348) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__ = &subdeviceCtrlCmdFbGetDynamicOfflinedPages_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__ = &subdeviceCtrlCmdFbGetDynamicOfflinedPages_92bfc3;
    }

    // subdeviceCtrlCmdMemSysGetStaticConfig -- halified (2 hals) exported (id=0x20800a1c) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysGetStaticConfig__ = &subdeviceCtrlCmdMemSysGetStaticConfig_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysGetStaticConfig__ = &subdeviceCtrlCmdMemSysGetStaticConfig_92bfc3;
    }

    // subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable -- halified (2 hals) exported (id=0x20800a6b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__ = &subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__ = &subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_92bfc3;
    }

    // subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration -- halified (2 hals) exported (id=0x20801355) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__ = &subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__ = &subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_92bfc3;
    }

    // subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration -- halified (2 hals) exported (id=0x20801356) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__ = &subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__ = &subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_92bfc3;
    }

    // subdeviceCtrlCmdMemSysQueryDramEncryptionStatus -- halified (2 hals) exported (id=0x20801358) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__ = &subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__ = &subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_92bfc3;
    }

    // subdeviceCtrlCmdFifoDisableChannelsForKeyRotation -- halified (2 hals) exported (id=0x2080111a) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_IMPL;
    }

    // subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2 -- halified (2 hals) exported (id=0x2080111b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_IMPL;
    }

    // subdeviceCtrlCmdFifoRotateKeys -- halified (2 hals) exported (id=0x2080111c) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoRotateKeys__ = &subdeviceCtrlCmdFifoRotateKeys_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoRotateKeys__ = &subdeviceCtrlCmdFifoRotateKeys_IMPL;
    }

    // subdeviceCtrlCmdFifoDisableUsermodeChannels -- halified (2 hals) exported (id=0x20801117) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoDisableUsermodeChannels__ = &subdeviceCtrlCmdFifoDisableUsermodeChannels_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoDisableUsermodeChannels__ = &subdeviceCtrlCmdFifoDisableUsermodeChannels_5baef9;
    }

    // subdeviceCtrlCmdFifoGetUserdLocation -- halified (2 hals) exported (id=0x2080110d) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__subdeviceCtrlCmdFifoGetUserdLocation__ = &subdeviceCtrlCmdFifoGetUserdLocation_IMPL;
    }
    // default
    else
    {
        pThis->__subdeviceCtrlCmdFifoGetUserdLocation__ = &subdeviceCtrlCmdFifoGetUserdLocation_395e98;
    }

    // subdeviceCtrlCmdFifoObjschedGetCaps -- halified (2 hals) exported (id=0x20801122) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoObjschedGetCaps__ = &subdeviceCtrlCmdFifoObjschedGetCaps_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoObjschedGetCaps__ = &subdeviceCtrlCmdFifoObjschedGetCaps_5baef9;
    }

    // subdeviceCtrlCmdFifoConfigCtxswTimeout -- halified (2 hals) exported (id=0x20801110) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoConfigCtxswTimeout__ = &subdeviceCtrlCmdFifoConfigCtxswTimeout_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoConfigCtxswTimeout__ = &subdeviceCtrlCmdFifoConfigCtxswTimeout_5baef9;
    }

    // subdeviceCtrlCmdFifoGetDeviceInfoTable -- halified (2 hals) exported (id=0x20801112) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoGetDeviceInfoTable__ = &subdeviceCtrlCmdFifoGetDeviceInfoTable_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoGetDeviceInfoTable__ = &subdeviceCtrlCmdFifoGetDeviceInfoTable_92bfc3;
    }

    // subdeviceCtrlCmdFifoUpdateChannelInfo -- halified (2 hals) exported (id=0x20801116) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoUpdateChannelInfo__ = &subdeviceCtrlCmdFifoUpdateChannelInfo_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoUpdateChannelInfo__ = &subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL;
    }

    // subdeviceCtrlCmdKGrCtxswPmMode -- halified (2 hals) exported (id=0x20801207) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdKGrCtxswPmMode__ = &subdeviceCtrlCmdKGrCtxswPmMode_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdKGrCtxswPmMode__ = &subdeviceCtrlCmdKGrCtxswPmMode_IMPL;
    }

    // subdeviceCtrlCmdGpuQueryEccStatus -- halified (2 hals) exported (id=0x2080012f) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccStatus__ = &subdeviceCtrlCmdGpuQueryEccStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccStatus__ = &subdeviceCtrlCmdGpuQueryEccStatus_92bfc3;
    }

    // subdeviceCtrlCmdGpuQueryIllumSupport -- halified (2 hals) exported (id=0x20800153) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryIllumSupport__ = &subdeviceCtrlCmdGpuQueryIllumSupport_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryIllumSupport__ = &subdeviceCtrlCmdGpuQueryIllumSupport_5baef9;
    }

    // subdeviceCtrlCmdGpuQueryScrubberStatus -- halified (2 hals) exported (id=0x2080015f) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryScrubberStatus__ = &subdeviceCtrlCmdGpuQueryScrubberStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryScrubberStatus__ = &subdeviceCtrlCmdGpuQueryScrubberStatus_92bfc3;
    }

    // subdeviceCtrlCmdGetP2pCaps -- halified (2 hals) exported (id=0x208001a0) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGetP2pCaps__ = &subdeviceCtrlCmdGetP2pCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGetP2pCaps__ = &subdeviceCtrlCmdGetP2pCaps_92bfc3;
    }

    // subdeviceCtrlCmdSetRcRecovery -- halified (2 hals) exported (id=0x2080220d) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdSetRcRecovery__ = &subdeviceCtrlCmdSetRcRecovery_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdSetRcRecovery__ = &subdeviceCtrlCmdSetRcRecovery_5baef9;
    }

    // subdeviceCtrlCmdGetRcRecovery -- halified (2 hals) exported (id=0x2080220e) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGetRcRecovery__ = &subdeviceCtrlCmdGetRcRecovery_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGetRcRecovery__ = &subdeviceCtrlCmdGetRcRecovery_5baef9;
    }

    // subdeviceCtrlCmdCeGetCaps -- halified (2 hals) exported (id=0x20802a01)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCeGetCaps__ = &subdeviceCtrlCmdCeGetCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCeGetCaps__ = &subdeviceCtrlCmdCeGetCaps_IMPL;
    }

    // subdeviceCtrlCmdCeGetCapsV2 -- halified (2 hals) exported (id=0x20802a03)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCeGetCapsV2__ = &subdeviceCtrlCmdCeGetCapsV2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCeGetCapsV2__ = &subdeviceCtrlCmdCeGetCapsV2_IMPL;
    }

    // subdeviceCtrlCmdCeGetAllCaps -- halified (2 hals) exported (id=0x20802a0a)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCeGetAllCaps__ = &subdeviceCtrlCmdCeGetAllCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCeGetAllCaps__ = &subdeviceCtrlCmdCeGetAllCaps_IMPL;
    }

    // subdeviceCtrlCmdGpuQueryEccConfiguration -- halified (2 hals) exported (id=0x20800133) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccConfiguration__ = &subdeviceCtrlCmdGpuQueryEccConfiguration_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccConfiguration__ = &subdeviceCtrlCmdGpuQueryEccConfiguration_92bfc3;
    }

    // subdeviceCtrlCmdGpuSetEccConfiguration -- halified (2 hals) exported (id=0x20800134) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuSetEccConfiguration__ = &subdeviceCtrlCmdGpuSetEccConfiguration_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuSetEccConfiguration__ = &subdeviceCtrlCmdGpuSetEccConfiguration_92bfc3;
    }

    // subdeviceCtrlCmdGpuResetEccErrorStatus -- halified (2 hals) exported (id=0x20800136) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuResetEccErrorStatus__ = &subdeviceCtrlCmdGpuResetEccErrorStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuResetEccErrorStatus__ = &subdeviceCtrlCmdGpuResetEccErrorStatus_92bfc3;
    }

    // subdeviceCtrlCmdGspGetFeatures -- halified (2 hals) exported (id=0x20803601) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGspGetFeatures__ = &subdeviceCtrlCmdGspGetFeatures_KERNEL;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGspGetFeatures__ = &subdeviceCtrlCmdGspGetFeatures_92bfc3;
    }

    // subdeviceCtrlCmdBifGetStaticInfo -- halified (2 hals) exported (id=0x20800aac) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBifGetStaticInfo__ = &subdeviceCtrlCmdBifGetStaticInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBifGetStaticInfo__ = &subdeviceCtrlCmdBifGetStaticInfo_92bfc3;
    }

    // subdeviceCtrlCmdInternalGetLocalAtsConfig -- halified (2 hals) exported (id=0x20800afb) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdInternalGetLocalAtsConfig__ = &subdeviceCtrlCmdInternalGetLocalAtsConfig_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdInternalGetLocalAtsConfig__ = &subdeviceCtrlCmdInternalGetLocalAtsConfig_92bfc3;
    }

    // subdeviceCtrlCmdInternalSetPeerAtsConfig -- halified (2 hals) exported (id=0x20800afc) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdInternalSetPeerAtsConfig__ = &subdeviceCtrlCmdInternalSetPeerAtsConfig_395e98;
    }
    else
    {
        pThis->__subdeviceCtrlCmdInternalSetPeerAtsConfig__ = &subdeviceCtrlCmdInternalSetPeerAtsConfig_92bfc3;
    }

    // subdeviceCtrlCmdCcuGetSampleInfo -- halified (2 hals) exported (id=0x20800ab2) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCcuGetSampleInfo__ = &subdeviceCtrlCmdCcuGetSampleInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCcuGetSampleInfo__ = &subdeviceCtrlCmdCcuGetSampleInfo_5baef9;
    }
} // End __nvoc_init_funcTable_Subdevice_1 with approximately 112 basic block(s).


// Initialize vtable(s) for 86 virtual method(s).
void __nvoc_init_funcTable_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 56 per-object function pointer(s).
    __nvoc_init_funcTable_Subdevice_1(pThis, pRmhalspecowner, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_Subdevice = pThis;    // (subdevice) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Subdevice;    // (subdevice) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Subdevice(pThis, pRmhalspecowner, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_Subdevice(Subdevice **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    Subdevice *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;
    GpuHalspecOwner *pGpuhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(Subdevice));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(Subdevice));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_Subdevice_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_Subdevice_cleanup);
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_Subdevice_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__Subdevice(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    __nvoc_status = __nvoc_ctor_Subdevice(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_Subdevice_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_Subdevice_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(Subdevice));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_Subdevice(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_Subdevice((Subdevice **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

