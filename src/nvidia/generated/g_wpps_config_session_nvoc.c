#define NVOC_WPPS_CONFIG_SESSION_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_wpps_config_session_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__853c55 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_WppsConfigSession;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for WppsConfigSession
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__WppsConfigSession(WppsConfigSession*);
void __nvoc_init_funcTable_WppsConfigSession(WppsConfigSession*);
NV_STATUS __nvoc_ctor_WppsConfigSession(WppsConfigSession*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_WppsConfigSession(WppsConfigSession*);
void __nvoc_dtor_WppsConfigSession(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__WppsConfigSession;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__WppsConfigSession;

// Down-thunk(s) to bridge WppsConfigSession methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_WppsConfigSession_resCanCopy(struct RsResource *pWppsConfigSession);    // this

// Up-thunk(s) to bridge WppsConfigSession methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionControl(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionMap(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionUnmap(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_wppsConfigSessionShareCallback(struct WppsConfigSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionGetRegBaseOffsetAndSize(struct WppsConfigSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionGetMapAddrSpace(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionInternalControlForward(struct WppsConfigSession *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_wppsConfigSessionGetInternalObjectHandle(struct WppsConfigSession *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_wppsConfigSessionAccessCallback(struct WppsConfigSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionGetMemInterMapParams(struct WppsConfigSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionCheckMemInterUnmap(struct WppsConfigSession *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionGetMemoryMappingDescriptor(struct WppsConfigSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionControlSerialization_Prologue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_wppsConfigSessionControlSerialization_Epilogue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionControl_Prologue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_wppsConfigSessionControl_Epilogue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionIsDuplicate(struct WppsConfigSession *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_wppsConfigSessionPreDestruct(struct WppsConfigSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionControlFilter(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_wppsConfigSessionIsPartialUnmapSupported(struct WppsConfigSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionMapTo(struct WppsConfigSession *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionUnmapFrom(struct WppsConfigSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_wppsConfigSessionGetRefCount(struct WppsConfigSession *pResource);    // this
void __nvoc_up_thunk_RsResource_wppsConfigSessionAddAdditionalDependants(struct RsClient *pClient, struct WppsConfigSession *pResource, RsResourceRef *pReference);    // this

// Class-specific details for WppsConfigSession
const struct NVOC_CLASS_DEF __nvoc_class_def_WppsConfigSession = 
{
    .classInfo.size =               sizeof(WppsConfigSession),
    .classInfo.classId =            classId(WppsConfigSession),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "WppsConfigSession",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_WppsConfigSession,
    .pCastInfo =          &__nvoc_castinfo__WppsConfigSession,
    .pExportInfo =        &__nvoc_export_info__WppsConfigSession
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS wppsConfigSessionCtrlCmdSetProfiles__EXPORT(void *pWppsConfigSession, void *pParams) {
    return wppsConfigSessionCtrlCmdSetProfiles_IMPL(pWppsConfigSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_WppsConfigSession[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &wppsConfigSessionCtrlCmdSetProfiles__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd0cd0101u,
        /*paramSize=*/  sizeof(NVD0CD_CTRL_SET_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_WppsConfigSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "wppsConfigSessionCtrlCmdSetProfiles"
#endif
    },
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__WppsConfigSession __nvoc_metadata__WppsConfigSession = {
    .rtti.pClassDef = &__nvoc_class_def_WppsConfigSession,    // (wppsConfigSession) this
    .rtti.dtor      = &__nvoc_dtor_WppsConfigSession,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__wppsConfigSessionCanCopy__ = &wppsConfigSessionCanCopy_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_WppsConfigSession_resCanCopy,    // virtual
    .vtable.__wppsConfigSessionControl__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__wppsConfigSessionMap__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__wppsConfigSessionUnmap__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__wppsConfigSessionShareCallback__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__wppsConfigSessionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__wppsConfigSessionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__wppsConfigSessionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__wppsConfigSessionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_wppsConfigSessionGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__wppsConfigSessionAccessCallback__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__wppsConfigSessionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__wppsConfigSessionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__wppsConfigSessionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__wppsConfigSessionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__wppsConfigSessionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__wppsConfigSessionControl_Prologue__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__wppsConfigSessionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_wppsConfigSessionControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__wppsConfigSessionIsDuplicate__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__wppsConfigSessionPreDestruct__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__wppsConfigSessionControlFilter__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__wppsConfigSessionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__wppsConfigSessionMapTo__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__wppsConfigSessionUnmapFrom__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__wppsConfigSessionGetRefCount__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__wppsConfigSessionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_wppsConfigSessionAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__WppsConfigSession = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__WppsConfigSession.rtti,    // [0]: (wppsConfigSession) this
        &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in WppsConfigSession from superclasses

// wppsConfigSessionCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_WppsConfigSession_resCanCopy(struct RsResource *pWppsConfigSession) {
    return wppsConfigSessionCanCopy((struct WppsConfigSession *)(((unsigned char *) pWppsConfigSession) - NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 24 up-thunk(s) defined to bridge methods in WppsConfigSession to superclasses

// wppsConfigSessionControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionControl(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// wppsConfigSessionMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionMap(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// wppsConfigSessionUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionUnmap(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// wppsConfigSessionShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_wppsConfigSessionShareCallback(struct WppsConfigSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// wppsConfigSessionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionGetRegBaseOffsetAndSize(struct WppsConfigSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// wppsConfigSessionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionGetMapAddrSpace(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// wppsConfigSessionInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_wppsConfigSessionInternalControlForward(struct WppsConfigSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)), command, pParams, size);
}

// wppsConfigSessionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_wppsConfigSessionGetInternalObjectHandle(struct WppsConfigSession *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource)));
}

// wppsConfigSessionAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_wppsConfigSessionAccessCallback(struct WppsConfigSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// wppsConfigSessionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionGetMemInterMapParams(struct WppsConfigSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// wppsConfigSessionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionCheckMemInterUnmap(struct WppsConfigSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// wppsConfigSessionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionGetMemoryMappingDescriptor(struct WppsConfigSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// wppsConfigSessionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionControlSerialization_Prologue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// wppsConfigSessionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_wppsConfigSessionControlSerialization_Epilogue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// wppsConfigSessionControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_wppsConfigSessionControl_Prologue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// wppsConfigSessionControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_wppsConfigSessionControl_Epilogue(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// wppsConfigSessionIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionIsDuplicate(struct WppsConfigSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// wppsConfigSessionPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_wppsConfigSessionPreDestruct(struct WppsConfigSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// wppsConfigSessionControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionControlFilter(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// wppsConfigSessionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_wppsConfigSessionIsPartialUnmapSupported(struct WppsConfigSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// wppsConfigSessionMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionMapTo(struct WppsConfigSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// wppsConfigSessionUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_wppsConfigSessionUnmapFrom(struct WppsConfigSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// wppsConfigSessionGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_wppsConfigSessionGetRefCount(struct WppsConfigSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// wppsConfigSessionAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_wppsConfigSessionAddAdditionalDependants(struct RsClient *pClient, struct WppsConfigSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(WppsConfigSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__WppsConfigSession = 
{
    .numEntries=     1,
    .pExportEntries= __nvoc_exported_method_def_WppsConfigSession
};


// Destruct WppsConfigSession object.
void __nvoc_wppsConfigSessionDestruct(WppsConfigSession*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_WppsConfigSession(Dynamic* pThis) {

    WppsConfigSession *__nvoc_this = (WppsConfigSession *) pThis;

// Call destructor.
    __nvoc_wppsConfigSessionDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_WppsConfigSession(WppsConfigSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct WppsConfigSession object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_WppsConfigSession(WppsConfigSession *pWppsConfigSession, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pWppsConfigSession->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_WppsConfigSession_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_WppsConfigSession(pWppsConfigSession);

    // Call the constructor for this class.
    status = __nvoc_wppsConfigSessionConstruct(pWppsConfigSession, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_WppsConfigSession_fail__init;
    goto __nvoc_ctor_WppsConfigSession_exit; // Success

    // Unwind on error.
__nvoc_ctor_WppsConfigSession_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pWppsConfigSession->__nvoc_base_GpuResource);
__nvoc_ctor_WppsConfigSession_fail_GpuResource:
__nvoc_ctor_WppsConfigSession_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_WppsConfigSession_1(WppsConfigSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_WppsConfigSession_1


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_WppsConfigSession(WppsConfigSession *pThis) {
    __nvoc_init_funcTable_WppsConfigSession_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__WppsConfigSession(WppsConfigSession *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_WppsConfigSession = pThis;    // (wppsConfigSession) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__WppsConfigSession.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__WppsConfigSession.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__WppsConfigSession;    // (wppsConfigSession) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_WppsConfigSession(pThis);
}

NV_STATUS __nvoc_objCreate_WppsConfigSession(WppsConfigSession **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    WppsConfigSession *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(WppsConfigSession));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(WppsConfigSession));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__WppsConfigSession(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_WppsConfigSession(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_WppsConfigSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_WppsConfigSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(WppsConfigSession));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_WppsConfigSession(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_WppsConfigSession((WppsConfigSession **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

