#define NVOC_KERNEL_CTXSHARE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_ctxshare_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x5ae2fe = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShare;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for KernelCtxShare
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__KernelCtxShare(KernelCtxShare*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelCtxShare(KernelCtxShare*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelCtxShare(KernelCtxShare*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelCtxShare(KernelCtxShare*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelCtxShare(KernelCtxShare*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCtxShare;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCtxShare;

// Down-thunk(s) to bridge KernelCtxShare methods from ancestors (if any)

// Up-thunk(s) to bridge KernelCtxShare methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShare = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCtxShare),
        /*classId=*/            classId(KernelCtxShare),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCtxShare",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCtxShare,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelCtxShare,
    /*pExportInfo=*/        &__nvoc_export_info__KernelCtxShare
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelCtxShare __nvoc_metadata__KernelCtxShare = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCtxShare,    // (kctxshare) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCtxShare,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(KernelCtxShare, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelCtxShare, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelCtxShare = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelCtxShare.rtti,    // [0]: (kctxshare) this
        &__nvoc_metadata__KernelCtxShare.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__KernelCtxShare.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCtxShare = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_KernelCtxShare(KernelCtxShare *pThis) {
    __nvoc_kctxshareDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCtxShare(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_KernelCtxShare(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShare_fail_RsShared;
    __nvoc_init_dataField_KernelCtxShare(pThis, pRmhalspecowner);

    status = __nvoc_kctxshareConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShare_fail__init;
    goto __nvoc_ctor_KernelCtxShare_exit; // Success

__nvoc_ctor_KernelCtxShare_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_KernelCtxShare_fail_RsShared:
__nvoc_ctor_KernelCtxShare_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCtxShare_1(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
} // End __nvoc_init_funcTable_KernelCtxShare_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelCtxShare(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelCtxShare_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelCtxShare(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_KernelCtxShare = pThis;    // (kctxshare) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShare.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShare.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShare;    // (kctxshare) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelCtxShare(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelCtxShare(KernelCtxShare **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelCtxShare *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCtxShare), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelCtxShare));

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelCtxShare(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCtxShare(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCtxShare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCtxShare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCtxShare));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShare(KernelCtxShare **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelCtxShare(ppThis, pParent, createFlags);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x1f9af1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShareApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for KernelCtxShareApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__KernelCtxShareApi(KernelCtxShareApi*);
void __nvoc_init_funcTable_KernelCtxShareApi(KernelCtxShareApi*);
NV_STATUS __nvoc_ctor_KernelCtxShareApi(KernelCtxShareApi*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_KernelCtxShareApi(KernelCtxShareApi*);
void __nvoc_dtor_KernelCtxShareApi(KernelCtxShareApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCtxShareApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCtxShareApi;

// Down-thunk(s) to bridge KernelCtxShareApi methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_KernelCtxShareApi_resCanCopy(struct RsResource *pKernelCtxShareApi);    // this

// Up-thunk(s) to bridge KernelCtxShareApi methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiControl(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiMap(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiUnmap(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_kctxshareapiShareCallback(struct KernelCtxShareApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiGetRegBaseOffsetAndSize(struct KernelCtxShareApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiGetMapAddrSpace(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiInternalControlForward(struct KernelCtxShareApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_kctxshareapiGetInternalObjectHandle(struct KernelCtxShareApi *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_kctxshareapiAccessCallback(struct KernelCtxShareApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiGetMemInterMapParams(struct KernelCtxShareApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiCheckMemInterUnmap(struct KernelCtxShareApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiGetMemoryMappingDescriptor(struct KernelCtxShareApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Prologue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Epilogue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiControl_Prologue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kctxshareapiControl_Epilogue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiIsDuplicate(struct KernelCtxShareApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_kctxshareapiPreDestruct(struct KernelCtxShareApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiControlFilter(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kctxshareapiIsPartialUnmapSupported(struct KernelCtxShareApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiMapTo(struct KernelCtxShareApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiUnmapFrom(struct KernelCtxShareApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_kctxshareapiGetRefCount(struct KernelCtxShareApi *pResource);    // this
void __nvoc_up_thunk_RsResource_kctxshareapiAddAdditionalDependants(struct RsClient *pClient, struct KernelCtxShareApi *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShareApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCtxShareApi),
        /*classId=*/            classId(KernelCtxShareApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCtxShareApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCtxShareApi,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelCtxShareApi,
    /*pExportInfo=*/        &__nvoc_export_info__KernelCtxShareApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelCtxShareApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90670102u,
        /*paramSize=*/  sizeof(NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCtxShareApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kctxshareapiCtrlCmdSetTpcPartitionTable"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kctxshareapiCtrlCmdGetCwdWatermark_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90670201u,
        /*paramSize=*/  sizeof(NV9067_CTRL_CWD_WATERMARK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCtxShareApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kctxshareapiCtrlCmdGetCwdWatermark"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kctxshareapiCtrlCmdSetCwdWatermark_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90670202u,
        /*paramSize=*/  sizeof(NV9067_CTRL_CWD_WATERMARK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCtxShareApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kctxshareapiCtrlCmdSetCwdWatermark"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelCtxShareApi __nvoc_metadata__KernelCtxShareApi = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCtxShareApi,    // (kctxshareapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCtxShareApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__kctxshareapiCanCopy__ = &kctxshareapiCanCopy_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_KernelCtxShareApi_resCanCopy,    // virtual
    .vtable.__kctxshareapiControl__ = &__nvoc_up_thunk_GpuResource_kctxshareapiControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__kctxshareapiMap__ = &__nvoc_up_thunk_GpuResource_kctxshareapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__kctxshareapiUnmap__ = &__nvoc_up_thunk_GpuResource_kctxshareapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__kctxshareapiShareCallback__ = &__nvoc_up_thunk_GpuResource_kctxshareapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__kctxshareapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kctxshareapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__kctxshareapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kctxshareapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__kctxshareapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kctxshareapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__kctxshareapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kctxshareapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__kctxshareapiAccessCallback__ = &__nvoc_up_thunk_RmResource_kctxshareapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__kctxshareapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kctxshareapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__kctxshareapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kctxshareapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__kctxshareapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kctxshareapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__kctxshareapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__kctxshareapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__kctxshareapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__kctxshareapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__kctxshareapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_kctxshareapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__kctxshareapiPreDestruct__ = &__nvoc_up_thunk_RsResource_kctxshareapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__kctxshareapiControlFilter__ = &__nvoc_up_thunk_RsResource_kctxshareapiControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__kctxshareapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kctxshareapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__kctxshareapiMapTo__ = &__nvoc_up_thunk_RsResource_kctxshareapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__kctxshareapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_kctxshareapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__kctxshareapiGetRefCount__ = &__nvoc_up_thunk_RsResource_kctxshareapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__kctxshareapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kctxshareapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelCtxShareApi = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__KernelCtxShareApi.rtti,    // [0]: (kctxshareapi) this
        &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in KernelCtxShareApi from superclasses

// kctxshareapiCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_KernelCtxShareApi_resCanCopy(struct RsResource *pKernelCtxShareApi) {
    return kctxshareapiCanCopy((struct KernelCtxShareApi *)(((unsigned char *) pKernelCtxShareApi) - NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 24 up-thunk(s) defined to bridge methods in KernelCtxShareApi to superclasses

// kctxshareapiControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiControl(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// kctxshareapiMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiMap(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// kctxshareapiUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiUnmap(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// kctxshareapiShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_kctxshareapiShareCallback(struct KernelCtxShareApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// kctxshareapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiGetRegBaseOffsetAndSize(struct KernelCtxShareApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// kctxshareapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiGetMapAddrSpace(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// kctxshareapiInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiInternalControlForward(struct KernelCtxShareApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)), command, pParams, size);
}

// kctxshareapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_kctxshareapiGetInternalObjectHandle(struct KernelCtxShareApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource)));
}

// kctxshareapiAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_kctxshareapiAccessCallback(struct KernelCtxShareApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// kctxshareapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiGetMemInterMapParams(struct KernelCtxShareApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// kctxshareapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiCheckMemInterUnmap(struct KernelCtxShareApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// kctxshareapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiGetMemoryMappingDescriptor(struct KernelCtxShareApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// kctxshareapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Prologue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kctxshareapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Epilogue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kctxshareapiControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiControl_Prologue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kctxshareapiControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kctxshareapiControl_Epilogue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kctxshareapiIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiIsDuplicate(struct KernelCtxShareApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// kctxshareapiPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kctxshareapiPreDestruct(struct KernelCtxShareApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kctxshareapiControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiControlFilter(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// kctxshareapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_kctxshareapiIsPartialUnmapSupported(struct KernelCtxShareApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kctxshareapiMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiMapTo(struct KernelCtxShareApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kctxshareapiUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiUnmapFrom(struct KernelCtxShareApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kctxshareapiGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_kctxshareapiGetRefCount(struct KernelCtxShareApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kctxshareapiAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kctxshareapiAddAdditionalDependants(struct RsClient *pClient, struct KernelCtxShareApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCtxShareApi = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelCtxShareApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_KernelCtxShareApi(KernelCtxShareApi *pThis) {
    __nvoc_kctxshareapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCtxShareApi(KernelCtxShareApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_KernelCtxShareApi(KernelCtxShareApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShareApi_fail_GpuResource;
    __nvoc_init_dataField_KernelCtxShareApi(pThis);

    status = __nvoc_kctxshareapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShareApi_fail__init;
    goto __nvoc_ctor_KernelCtxShareApi_exit; // Success

__nvoc_ctor_KernelCtxShareApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelCtxShareApi_fail_GpuResource:
__nvoc_ctor_KernelCtxShareApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCtxShareApi_1(KernelCtxShareApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // kctxshareapiCtrlCmdSetTpcPartitionTable -- exported (id=0x90670102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__kctxshareapiCtrlCmdSetTpcPartitionTable__ = &kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL;
#endif

    // kctxshareapiCtrlCmdGetCwdWatermark -- exported (id=0x90670201)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__kctxshareapiCtrlCmdGetCwdWatermark__ = &kctxshareapiCtrlCmdGetCwdWatermark_IMPL;
#endif

    // kctxshareapiCtrlCmdSetCwdWatermark -- exported (id=0x90670202)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__kctxshareapiCtrlCmdSetCwdWatermark__ = &kctxshareapiCtrlCmdSetCwdWatermark_IMPL;
#endif
} // End __nvoc_init_funcTable_KernelCtxShareApi_1 with approximately 3 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_KernelCtxShareApi(KernelCtxShareApi *pThis) {

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_KernelCtxShareApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__KernelCtxShareApi(KernelCtxShareApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_KernelCtxShareApi = pThis;    // (kctxshareapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShareApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelCtxShareApi;    // (kctxshareapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelCtxShareApi(pThis);
}

NV_STATUS __nvoc_objCreate_KernelCtxShareApi(KernelCtxShareApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelCtxShareApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCtxShareApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelCtxShareApi));

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

    __nvoc_init__KernelCtxShareApi(pThis);
    status = __nvoc_ctor_KernelCtxShareApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCtxShareApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCtxShareApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCtxShareApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShareApi(KernelCtxShareApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelCtxShareApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

