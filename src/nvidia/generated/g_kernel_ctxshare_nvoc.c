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
char __nvoc_class_id_uniqueness_check__5ae2fe = 1;
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
void __nvoc_dtor_KernelCtxShare(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCtxShare;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCtxShare;

// Down-thunk(s) to bridge KernelCtxShare methods from ancestors (if any)

// Up-thunk(s) to bridge KernelCtxShare methods to ancestors (if any)

// Class-specific details for KernelCtxShare
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShare = 
{
    .classInfo.size =               sizeof(KernelCtxShare),
    .classInfo.classId =            classId(KernelCtxShare),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelCtxShare",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_KernelCtxShare,
    .pCastInfo =          &__nvoc_castinfo__KernelCtxShare,
    .pExportInfo =        &__nvoc_export_info__KernelCtxShare
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelCtxShare __nvoc_metadata__KernelCtxShare = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCtxShare,    // (kctxshare) this
    .rtti.dtor      = &__nvoc_dtor_KernelCtxShare,
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
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelCtxShare object.
void __nvoc_kctxshareDestruct(KernelCtxShare*);
void __nvoc_dtor_RsShared(Dynamic*);
void __nvoc_dtor_KernelCtxShare(Dynamic* pThis) {

    KernelCtxShare *__nvoc_this = (KernelCtxShare *) pThis;

// Call destructor.
    __nvoc_kctxshareDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_RsShared((Dynamic *) &__nvoc_this->__nvoc_base_RsShared);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelCtxShare(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct KernelCtxShare object.
NV_STATUS __nvoc_ctor_RsShared(RsShared *pShared);
NV_STATUS __nvoc_ctor_KernelCtxShare(KernelCtxShare *pKernelCtxShare, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsShared(&pKernelCtxShare->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShare_fail_RsShared;

    // Initialize data fields.
    __nvoc_init_dataField_KernelCtxShare(pKernelCtxShare, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_kctxshareConstruct(pKernelCtxShare);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShare_fail__init;
    goto __nvoc_ctor_KernelCtxShare_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelCtxShare_fail__init:
    __nvoc_dtor_RsShared((Dynamic *)&pKernelCtxShare->__nvoc_base_RsShared);
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

NV_STATUS __nvoc_objCreate_KernelCtxShare(KernelCtxShare **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelCtxShare *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelCtxShare));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelCtxShare));

    __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelCtxShare_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelCtxShare_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelCtxShare(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelCtxShare(__nvoc_pThis, pRmhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelCtxShare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelCtxShare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelCtxShare));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShare(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_KernelCtxShare((KernelCtxShare **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__1f9af1 = 1;
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
NV_STATUS __nvoc_ctor_KernelCtxShareApi(KernelCtxShareApi*, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_KernelCtxShareApi(KernelCtxShareApi*);
void __nvoc_dtor_KernelCtxShareApi(Dynamic*);

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
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NvBool __nvoc_down_thunk_KernelCtxShareApi_resCanCopy(struct RsResource *pKernelCtxShareApi);    // this

// Up-thunk(s) to bridge KernelCtxShareApi methods to ancestors (if any)

// Class-specific details for KernelCtxShareApi
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShareApi = 
{
    .classInfo.size =               sizeof(KernelCtxShareApi),
    .classInfo.classId =            classId(KernelCtxShareApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelCtxShareApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_KernelCtxShareApi,
    .pCastInfo =          &__nvoc_castinfo__KernelCtxShareApi,
    .pExportInfo =        &__nvoc_export_info__KernelCtxShareApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS kctxshareapiCtrlCmdSetTpcPartitionTable__EXPORT(void *pKernelCtxShareApi, void *pParams) {
    return kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL(pKernelCtxShareApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS kctxshareapiCtrlCmdGetCwdWatermark__EXPORT(void *pKernelCtxShareApi, void *pParams) {
    return kctxshareapiCtrlCmdGetCwdWatermark_IMPL(pKernelCtxShareApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS kctxshareapiCtrlCmdSetCwdWatermark__EXPORT(void *pKernelCtxShareApi, void *pParams) {
    return kctxshareapiCtrlCmdSetCwdWatermark_IMPL(pKernelCtxShareApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelCtxShareApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &kctxshareapiCtrlCmdSetTpcPartitionTable__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &kctxshareapiCtrlCmdGetCwdWatermark__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &kctxshareapiCtrlCmdSetCwdWatermark__EXPORT,
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


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelCtxShareApi __nvoc_metadata__KernelCtxShareApi = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCtxShareApi,    // (kctxshareapi) this
    .rtti.dtor      = &__nvoc_dtor_KernelCtxShareApi,
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

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_KernelCtxShareApi_resCanCopy,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
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
    return kctxshareapiCanCopy_IMPL((struct KernelCtxShareApi *)(((unsigned char *) pKernelCtxShareApi) - NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCtxShareApi = 
{
    .numEntries=     3,
    .pExportEntries= __nvoc_exported_method_def_KernelCtxShareApi
};


// Destruct KernelCtxShareApi object.
void __nvoc_kctxshareapiDestruct(KernelCtxShareApi*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_KernelCtxShareApi(Dynamic* pThis) {

    KernelCtxShareApi *__nvoc_this = (KernelCtxShareApi *) pThis;

// Call destructor.
    __nvoc_kctxshareapiDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelCtxShareApi(KernelCtxShareApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct KernelCtxShareApi object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_KernelCtxShareApi(KernelCtxShareApi *pKernelCtxShareApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pKernelCtxShareApi->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShareApi_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_KernelCtxShareApi(pKernelCtxShareApi);

    // Call the constructor for this class.
    status = __nvoc_kctxshareapiConstruct(pKernelCtxShareApi, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCtxShareApi_fail__init;
    goto __nvoc_ctor_KernelCtxShareApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelCtxShareApi_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pKernelCtxShareApi->__nvoc_base_GpuResource);
__nvoc_ctor_KernelCtxShareApi_fail_GpuResource:
__nvoc_ctor_KernelCtxShareApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCtxShareApi_1(KernelCtxShareApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelCtxShareApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelCtxShareApi(KernelCtxShareApi *pThis) {
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

NV_STATUS __nvoc_objCreate_KernelCtxShareApi(KernelCtxShareApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelCtxShareApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelCtxShareApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelCtxShareApi));

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
    __nvoc_init__KernelCtxShareApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_KernelCtxShareApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelCtxShareApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelCtxShareApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelCtxShareApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShareApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_KernelCtxShareApi((KernelCtxShareApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

