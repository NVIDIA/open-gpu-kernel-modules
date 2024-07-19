#define NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_graphics_context_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7ead09 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContext;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelGraphicsContext(KernelGraphicsContext*, RmHalspecOwner* );
void __nvoc_dtor_KernelGraphicsContext(KernelGraphicsContext*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsContext;

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContext_KernelGraphicsContext = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGraphicsContext,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsContext,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContext_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContext_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContext_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContext_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContext_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContext, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGraphicsContext = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGraphicsContext_KernelGraphicsContext,
        &__nvoc_rtti_KernelGraphicsContext_GpuResource,
        &__nvoc_rtti_KernelGraphicsContext_RmResource,
        &__nvoc_rtti_KernelGraphicsContext_RmResourceCommon,
        &__nvoc_rtti_KernelGraphicsContext_RsResource,
        &__nvoc_rtti_KernelGraphicsContext_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGraphicsContext,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGraphicsContext
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

};

// 2 down-thunk(s) defined to bridge methods in KernelGraphicsContext from superclasses

// kgrctxCanCopy: inline virtual override (res) base (gpures) body
static NvBool __nvoc_down_thunk_KernelGraphicsContext_resCanCopy(struct RsResource *arg_this) {
    return kgrctxCanCopy((struct KernelGraphicsContext *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelGraphicsContext_RsResource.offset));
}

// kgrctxGetInternalObjectHandle: virtual override (gpures) base (gpures)
static NvHandle __nvoc_down_thunk_KernelGraphicsContext_gpuresGetInternalObjectHandle(struct GpuResource *arg_this) {
    return kgrctxGetInternalObjectHandle((struct KernelGraphicsContext *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelGraphicsContext_GpuResource.offset));
}


// 23 up-thunk(s) defined to bridge methods in KernelGraphicsContext to superclasses

// kgrctxControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxControl(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), pCallContext, pParams);
}

// kgrctxMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxMap(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// kgrctxUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxUnmap(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), pCallContext, pCpuMapping);
}

// kgrctxShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_kgrctxShareCallback(struct KernelGraphicsContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// kgrctxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxGetRegBaseOffsetAndSize(struct KernelGraphicsContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), pGpu, pOffset, pSize);
}

// kgrctxGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxGetMapAddrSpace(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// kgrctxInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kgrctxInternalControlForward(struct KernelGraphicsContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelGraphicsContext_GpuResource.offset), command, pParams, size);
}

// kgrctxAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_kgrctxAccessCallback(struct KernelGraphicsContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// kgrctxGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kgrctxGetMemInterMapParams(struct KernelGraphicsContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), pParams);
}

// kgrctxCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kgrctxCheckMemInterUnmap(struct KernelGraphicsContext *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), bSubdeviceHandleProvided);
}

// kgrctxGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kgrctxGetMemoryMappingDescriptor(struct KernelGraphicsContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), ppMemDesc);
}

// kgrctxControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kgrctxControlSerialization_Prologue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), pCallContext, pParams);
}

// kgrctxControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_kgrctxControlSerialization_Epilogue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), pCallContext, pParams);
}

// kgrctxControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kgrctxControl_Prologue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), pCallContext, pParams);
}

// kgrctxControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_kgrctxControl_Epilogue(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RmResource.offset), pCallContext, pParams);
}

// kgrctxIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kgrctxIsDuplicate(struct KernelGraphicsContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset), hMemory, pDuplicate);
}

// kgrctxPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_kgrctxPreDestruct(struct KernelGraphicsContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset));
}

// kgrctxControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kgrctxControlFilter(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset), pCallContext, pParams);
}

// kgrctxIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_kgrctxIsPartialUnmapSupported(struct KernelGraphicsContext *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset));
}

// kgrctxMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kgrctxMapTo(struct KernelGraphicsContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset), pParams);
}

// kgrctxUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kgrctxUnmapFrom(struct KernelGraphicsContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset), pParams);
}

// kgrctxGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_kgrctxGetRefCount(struct KernelGraphicsContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset));
}

// kgrctxAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_kgrctxAddAdditionalDependants(struct RsClient *pClient, struct KernelGraphicsContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelGraphicsContext_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsContext = 
{
    /*numEntries=*/     4,
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

    // kgrctxCanCopy -- inline virtual override (res) base (gpures) body
    pThis->__kgrctxCanCopy__ = &kgrctxCanCopy_0c883b;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_KernelGraphicsContext_resCanCopy;

    // kgrctxGetInternalObjectHandle -- virtual override (gpures) base (gpures)
    pThis->__kgrctxGetInternalObjectHandle__ = &kgrctxGetInternalObjectHandle_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__ = &__nvoc_down_thunk_KernelGraphicsContext_gpuresGetInternalObjectHandle;

    // kgrctxShouldPreAllocPmBuffer -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */  && (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ ))
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

    // kgrctxControl -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxControl__ = &__nvoc_up_thunk_GpuResource_kgrctxControl;

    // kgrctxMap -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxMap__ = &__nvoc_up_thunk_GpuResource_kgrctxMap;

    // kgrctxUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxUnmap__ = &__nvoc_up_thunk_GpuResource_kgrctxUnmap;

    // kgrctxShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxShareCallback__ = &__nvoc_up_thunk_GpuResource_kgrctxShareCallback;

    // kgrctxGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kgrctxGetRegBaseOffsetAndSize;

    // kgrctxGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kgrctxGetMapAddrSpace;

    // kgrctxInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__kgrctxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kgrctxInternalControlForward;

    // kgrctxAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxAccessCallback__ = &__nvoc_up_thunk_RmResource_kgrctxAccessCallback;

    // kgrctxGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kgrctxGetMemInterMapParams;

    // kgrctxCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kgrctxCheckMemInterUnmap;

    // kgrctxGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kgrctxGetMemoryMappingDescriptor;

    // kgrctxControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kgrctxControlSerialization_Prologue;

    // kgrctxControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kgrctxControlSerialization_Epilogue;

    // kgrctxControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxControl_Prologue__ = &__nvoc_up_thunk_RmResource_kgrctxControl_Prologue;

    // kgrctxControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__kgrctxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kgrctxControl_Epilogue;

    // kgrctxIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__kgrctxIsDuplicate__ = &__nvoc_up_thunk_RsResource_kgrctxIsDuplicate;

    // kgrctxPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__kgrctxPreDestruct__ = &__nvoc_up_thunk_RsResource_kgrctxPreDestruct;

    // kgrctxControlFilter -- virtual inherited (res) base (gpures)
    pThis->__kgrctxControlFilter__ = &__nvoc_up_thunk_RsResource_kgrctxControlFilter;

    // kgrctxIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__kgrctxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kgrctxIsPartialUnmapSupported;

    // kgrctxMapTo -- virtual inherited (res) base (gpures)
    pThis->__kgrctxMapTo__ = &__nvoc_up_thunk_RsResource_kgrctxMapTo;

    // kgrctxUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__kgrctxUnmapFrom__ = &__nvoc_up_thunk_RsResource_kgrctxUnmapFrom;

    // kgrctxGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__kgrctxGetRefCount__ = &__nvoc_up_thunk_RsResource_kgrctxGetRefCount;

    // kgrctxAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__kgrctxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kgrctxAddAdditionalDependants;
} // End __nvoc_init_funcTable_KernelGraphicsContext_1 with approximately 35 basic block(s).


// Initialize vtable(s) for 31 virtual method(s).
void __nvoc_init_funcTable_KernelGraphicsContext(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 31 per-object function pointer(s).
    __nvoc_init_funcTable_KernelGraphicsContext_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_KernelGraphicsContext(KernelGraphicsContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGraphicsContext = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGraphicsContext);

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

    __nvoc_init_KernelGraphicsContext(pThis, pRmhalspecowner);
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
char __nvoc_class_id_uniqueness_check_0xe7abeb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContextShared;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_KernelGraphicsContextShared(KernelGraphicsContextShared*);
void __nvoc_init_funcTable_KernelGraphicsContextShared(KernelGraphicsContextShared*);
NV_STATUS __nvoc_ctor_KernelGraphicsContextShared(KernelGraphicsContextShared*);
void __nvoc_init_dataField_KernelGraphicsContextShared(KernelGraphicsContextShared*);
void __nvoc_dtor_KernelGraphicsContextShared(KernelGraphicsContextShared*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsContextShared;

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContextShared_KernelGraphicsContextShared = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGraphicsContextShared,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsContextShared,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContextShared_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContextShared, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsContextShared_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsContextShared, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGraphicsContextShared = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGraphicsContextShared_KernelGraphicsContextShared,
        &__nvoc_rtti_KernelGraphicsContextShared_RsShared,
        &__nvoc_rtti_KernelGraphicsContextShared_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGraphicsContextShared,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGraphicsContextShared
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsContextShared = 
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

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_KernelGraphicsContextShared(KernelGraphicsContextShared *pThis) {
    pThis->__nvoc_pbase_KernelGraphicsContextShared = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGraphicsContextShared);

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

    __nvoc_init_KernelGraphicsContextShared(pThis);
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

