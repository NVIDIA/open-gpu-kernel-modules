#define NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_graphics_context_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x7ead09 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContext;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for KernelGraphicsContext
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelGraphicsContext(KernelGraphicsContext*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphicsContext;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphicsContext;

// Down-thunk(s) to bridge KernelGraphicsContext methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_KernelGraphicsContext_resCanCopy(struct RsResource *arg_this);    // this
NvHandle __nvoc_down_thunk_KernelGraphicsContext_gpuresGetInternalObjectHandle(struct GpuResource *arg_this);    // this

// Up-thunk(s) to bridge KernelGraphicsContext methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxControl(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxMap(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxUnmap(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_kgrctxShareCallback(struct KernelGraphicsContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxGetRegBaseOffsetAndSize(struct KernelGraphicsContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxGetMapAddrSpace(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxInternalControlForward(struct KernelGraphicsContext *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvBool __nvoc_up_thunk_RmResource_kgrctxAccessCallback(struct KernelGraphicsContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxGetMemInterMapParams(struct KernelGraphicsContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxCheckMemInterUnmap(struct KernelGraphicsContext *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxGetMemoryMappingDescriptor(struct KernelGraphicsContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxControlSerialization_Prologue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kgrctxControlSerialization_Epilogue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxControl_Prologue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kgrctxControl_Epilogue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxIsDuplicate(struct KernelGraphicsContext *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_kgrctxPreDestruct(struct KernelGraphicsContext *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxControlFilter(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kgrctxIsPartialUnmapSupported(struct KernelGraphicsContext *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxMapTo(struct KernelGraphicsContext *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxUnmapFrom(struct KernelGraphicsContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_kgrctxGetRefCount(struct KernelGraphicsContext *pResource);    // this
void __nvoc_up_thunk_RsResource_kgrctxAddAdditionalDependants(struct RsClient *pClient, struct KernelGraphicsContext *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGraphicsContext),
        /*classId=*/            classId(KernelGraphicsContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGraphicsContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGraphicsContext,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelGraphicsContext,
    /*pExportInfo=*/        &__nvoc_export_info__KernelGraphicsContext
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelGraphicsContext[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kgrctxCtrlSetTpcPartitionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900101u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelGraphicsContext.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kgrctxCtrlSetTpcPartitionMode"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kgrctxCtrlGetTpcPartitionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900103u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelGraphicsContext.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kgrctxCtrlGetTpcPartitionMode"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kgrctxCtrlGetMMUDebugMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900105u,
        /*paramSize=*/  sizeof(NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelGraphicsContext.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kgrctxCtrlGetMMUDebugMode"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kgrctxCtrlProgramVidmemPromote_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900107u,
        /*paramSize=*/  sizeof(NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelGraphicsContext.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kgrctxCtrlProgramVidmemPromote"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kgrctxCtrlSetLgSectorPromotion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90010bu,
        /*paramSize=*/  sizeof(NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelGraphicsContext.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kgrctxCtrlSetLgSectorPromotion"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelGraphicsContext __nvoc_metadata__KernelGraphicsContext = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGraphicsContext,    // (kgrctx) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsContext,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__kgrctxCanCopy__ = &kgrctxCanCopy_e661f0,    // inline virtual override (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_KernelGraphicsContext_resCanCopy,    // virtual
    .vtable.__kgrctxGetInternalObjectHandle__ = &kgrctxGetInternalObjectHandle_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &__nvoc_down_thunk_KernelGraphicsContext_gpuresGetInternalObjectHandle,    // virtual
    .vtable.__kgrctxControl__ = &__nvoc_up_thunk_GpuResource_kgrctxControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__kgrctxMap__ = &__nvoc_up_thunk_GpuResource_kgrctxMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__kgrctxUnmap__ = &__nvoc_up_thunk_GpuResource_kgrctxUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__kgrctxShareCallback__ = &__nvoc_up_thunk_GpuResource_kgrctxShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__kgrctxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kgrctxGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__kgrctxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kgrctxGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__kgrctxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kgrctxInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__kgrctxAccessCallback__ = &__nvoc_up_thunk_RmResource_kgrctxAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__kgrctxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kgrctxGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__kgrctxCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kgrctxCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__kgrctxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kgrctxGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__kgrctxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kgrctxControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__kgrctxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kgrctxControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__kgrctxControl_Prologue__ = &__nvoc_up_thunk_RmResource_kgrctxControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__kgrctxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kgrctxControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__kgrctxIsDuplicate__ = &__nvoc_up_thunk_RsResource_kgrctxIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__kgrctxPreDestruct__ = &__nvoc_up_thunk_RsResource_kgrctxPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__kgrctxControlFilter__ = &__nvoc_up_thunk_RsResource_kgrctxControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__kgrctxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kgrctxIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__kgrctxMapTo__ = &__nvoc_up_thunk_RsResource_kgrctxMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__kgrctxUnmapFrom__ = &__nvoc_up_thunk_RsResource_kgrctxUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__kgrctxGetRefCount__ = &__nvoc_up_thunk_RsResource_kgrctxGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__kgrctxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kgrctxAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphicsContext = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__KernelGraphicsContext.rtti,    // [0]: (kgrctx) this
        &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelGraphicsContext from superclasses

// kgrctxCanCopy: inline virtual override (res) base (gpures) body
NvBool __nvoc_down_thunk_KernelGraphicsContext_resCanCopy(struct RsResource *arg_this) {
    return kgrctxCanCopy((struct KernelGraphicsContext *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kgrctxGetInternalObjectHandle: virtual override (gpures) base (gpures)
NvHandle __nvoc_down_thunk_KernelGraphicsContext_gpuresGetInternalObjectHandle(struct GpuResource *arg_this) {
    return kgrctxGetInternalObjectHandle((struct KernelGraphicsContext *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)));
}


// 23 up-thunk(s) defined to bridge methods in KernelGraphicsContext to superclasses

// kgrctxControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxControl(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// kgrctxMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxMap(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// kgrctxUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxUnmap(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// kgrctxShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_kgrctxShareCallback(struct KernelGraphicsContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// kgrctxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxGetRegBaseOffsetAndSize(struct KernelGraphicsContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// kgrctxGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxGetMapAddrSpace(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// kgrctxInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxInternalControlForward(struct KernelGraphicsContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource)), command, pParams, size);
}

// kgrctxAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_kgrctxAccessCallback(struct KernelGraphicsContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// kgrctxGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxGetMemInterMapParams(struct KernelGraphicsContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// kgrctxCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxCheckMemInterUnmap(struct KernelGraphicsContext *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// kgrctxGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxGetMemoryMappingDescriptor(struct KernelGraphicsContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// kgrctxControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxControlSerialization_Prologue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kgrctxControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kgrctxControlSerialization_Epilogue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kgrctxControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kgrctxControl_Prologue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kgrctxControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kgrctxControl_Epilogue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kgrctxIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxIsDuplicate(struct KernelGraphicsContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// kgrctxPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kgrctxPreDestruct(struct KernelGraphicsContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kgrctxControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxControlFilter(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// kgrctxIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_kgrctxIsPartialUnmapSupported(struct KernelGraphicsContext *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kgrctxMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxMapTo(struct KernelGraphicsContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kgrctxUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kgrctxUnmapFrom(struct KernelGraphicsContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kgrctxGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_kgrctxGetRefCount(struct KernelGraphicsContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kgrctxAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kgrctxAddAdditionalDependants(struct RsClient *pClient, struct KernelGraphicsContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphicsContext = 
{
    /*numEntries=*/     5,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelGraphicsContext
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_KernelGraphicsContext(KernelGraphicsContext *pThis) {
    __nvoc_kgrctxDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGraphicsContext(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_KernelGraphicsContext(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsContext_fail_GpuResource;
    __nvoc_init_dataField_KernelGraphicsContext(pThis, pRmhalspecowner);

    status = __nvoc_kgrctxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsContext_fail__init;
    goto __nvoc_ctor_KernelGraphicsContext_exit; // Success

__nvoc_ctor_KernelGraphicsContext_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelGraphicsContext_fail_GpuResource:
__nvoc_ctor_KernelGraphicsContext_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGraphicsContext_1(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kgrctxShouldPreAllocPmBuffer -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */  && (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ ))
    {
        pThis->__kgrctxShouldPreAllocPmBuffer__ = &kgrctxShouldPreAllocPmBuffer_VF;
    }
    else
    {
        pThis->__kgrctxShouldPreAllocPmBuffer__ = &kgrctxShouldPreAllocPmBuffer_PF;
    }

    // kgrctxGetRegisterAccessMapId -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgrctxGetRegisterAccessMapId__ = &kgrctxGetRegisterAccessMapId_aa21e9;
    }
    // default
    else
    {
        pThis->__kgrctxGetRegisterAccessMapId__ = &kgrctxGetRegisterAccessMapId_IMPL;
    }

    // kgrctxCtrlGetTpcPartitionMode -- exported (id=0x900103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kgrctxCtrlGetTpcPartitionMode__ = &kgrctxCtrlGetTpcPartitionMode_IMPL;
#endif

    // kgrctxCtrlSetTpcPartitionMode -- exported (id=0x900101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kgrctxCtrlSetTpcPartitionMode__ = &kgrctxCtrlSetTpcPartitionMode_IMPL;
#endif

    // kgrctxCtrlGetMMUDebugMode -- exported (id=0x900105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kgrctxCtrlGetMMUDebugMode__ = &kgrctxCtrlGetMMUDebugMode_IMPL;
#endif

    // kgrctxCtrlProgramVidmemPromote -- exported (id=0x900107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kgrctxCtrlProgramVidmemPromote__ = &kgrctxCtrlProgramVidmemPromote_IMPL;
#endif

    // kgrctxCtrlSetLgSectorPromotion -- exported (id=0x90010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kgrctxCtrlSetLgSectorPromotion__ = &kgrctxCtrlSetLgSectorPromotion_IMPL;
#endif
} // End __nvoc_init_funcTable_KernelGraphicsContext_1 with approximately 9 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_KernelGraphicsContext(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 7 per-object function pointer(s).
    __nvoc_init_funcTable_KernelGraphicsContext_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelGraphicsContext(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_KernelGraphicsContext = pThis;    // (kgrctx) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContext.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContext;    // (kgrctx) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelGraphicsContext(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGraphicsContext(KernelGraphicsContext **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGraphicsContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGraphicsContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGraphicsContext));

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelGraphicsContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGraphicsContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGraphicsContext_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGraphicsContext_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGraphicsContext));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsContext(KernelGraphicsContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelGraphicsContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xe7abeb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContextShared;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for KernelGraphicsContextShared
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__KernelGraphicsContextShared(KernelGraphicsContextShared*);
void __nvoc_init_funcTable_KernelGraphicsContextShared(KernelGraphicsContextShared*);
NV_STATUS __nvoc_ctor_KernelGraphicsContextShared(KernelGraphicsContextShared*);
void __nvoc_init_dataField_KernelGraphicsContextShared(KernelGraphicsContextShared*);
void __nvoc_dtor_KernelGraphicsContextShared(KernelGraphicsContextShared*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphicsContextShared;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphicsContextShared;

// Down-thunk(s) to bridge KernelGraphicsContextShared methods from ancestors (if any)

// Up-thunk(s) to bridge KernelGraphicsContextShared methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContextShared = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGraphicsContextShared),
        /*classId=*/            classId(KernelGraphicsContextShared),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGraphicsContextShared",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGraphicsContextShared,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelGraphicsContextShared,
    /*pExportInfo=*/        &__nvoc_export_info__KernelGraphicsContextShared
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelGraphicsContextShared __nvoc_metadata__KernelGraphicsContextShared = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGraphicsContextShared,    // (shrkgrctx) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsContextShared,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(KernelGraphicsContextShared, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGraphicsContextShared, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphicsContextShared = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelGraphicsContextShared.rtti,    // [0]: (shrkgrctx) this
        &__nvoc_metadata__KernelGraphicsContextShared.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__KernelGraphicsContextShared.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphicsContextShared = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_KernelGraphicsContextShared(KernelGraphicsContextShared *pThis) {
    __nvoc_shrkgrctxDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGraphicsContextShared(KernelGraphicsContextShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_KernelGraphicsContextShared(KernelGraphicsContextShared *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsContextShared_fail_RsShared;
    __nvoc_init_dataField_KernelGraphicsContextShared(pThis);

    status = __nvoc_shrkgrctxConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsContextShared_fail__init;
    goto __nvoc_ctor_KernelGraphicsContextShared_exit; // Success

__nvoc_ctor_KernelGraphicsContextShared_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_KernelGraphicsContextShared_fail_RsShared:
__nvoc_ctor_KernelGraphicsContextShared_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGraphicsContextShared_1(KernelGraphicsContextShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelGraphicsContextShared_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelGraphicsContextShared(KernelGraphicsContextShared *pThis) {
    __nvoc_init_funcTable_KernelGraphicsContextShared_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__KernelGraphicsContextShared(KernelGraphicsContextShared *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_KernelGraphicsContextShared = pThis;    // (shrkgrctx) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContextShared.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContextShared.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsContextShared;    // (shrkgrctx) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelGraphicsContextShared(pThis);
}

NV_STATUS __nvoc_objCreate_KernelGraphicsContextShared(KernelGraphicsContextShared **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGraphicsContextShared *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGraphicsContextShared), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGraphicsContextShared));

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

    __nvoc_init__KernelGraphicsContextShared(pThis);
    status = __nvoc_ctor_KernelGraphicsContextShared(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGraphicsContextShared_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGraphicsContextShared_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGraphicsContextShared));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsContextShared(KernelGraphicsContextShared **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGraphicsContextShared(ppThis, pParent, createFlags);

    return status;
}

