#define NVOC_CE_UTILS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_ce_utils_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__8b8bae = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for CeUtils
void __nvoc_init__Object(Object*);
void __nvoc_init__CeUtils(CeUtils*);
void __nvoc_init_funcTable_CeUtils(CeUtils*);
NV_STATUS __nvoc_ctor_CeUtils(CeUtils*, struct OBJGPU *pGpu, KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS *pAllocParams);
void __nvoc_init_dataField_CeUtils(CeUtils*);
void __nvoc_dtor_CeUtils(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CeUtils;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CeUtils;

// Down-thunk(s) to bridge CeUtils methods from ancestors (if any)

// Up-thunk(s) to bridge CeUtils methods to ancestors (if any)

// Class-specific details for CeUtils
const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils = 
{
    .classInfo.size =               sizeof(CeUtils),
    .classInfo.classId =            classId(CeUtils),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CeUtils",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_CeUtils,
    .pCastInfo =          &__nvoc_castinfo__CeUtils,
    .pExportInfo =        &__nvoc_export_info__CeUtils
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CeUtils __nvoc_metadata__CeUtils = {
    .rtti.pClassDef = &__nvoc_class_def_CeUtils,    // (ceutils) this
    .rtti.dtor      = &__nvoc_dtor_CeUtils,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(CeUtils, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CeUtils = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__CeUtils.rtti,    // [0]: (ceutils) this
        &__nvoc_metadata__CeUtils.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CeUtils = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CeUtils object.
void __nvoc_ceutilsDestruct(CeUtils*);
void __nvoc_dtor_Object(Dynamic*);
void __nvoc_dtor_CeUtils(Dynamic* pThis) {

    CeUtils *__nvoc_this = (CeUtils *) pThis;

// Call destructor.
    __nvoc_ceutilsDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_Object((Dynamic *) &__nvoc_this->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_CeUtils(CeUtils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct CeUtils object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_CeUtils(CeUtils *pCeUtils, struct OBJGPU *pGpu, KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS *pAllocParams) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pCeUtils->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CeUtils_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_CeUtils(pCeUtils);

    // Call the constructor for this class.
    status = __nvoc_ceutilsConstruct(pCeUtils, pGpu, pKernelMIGGPUInstance, pAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtils_fail__init;
    goto __nvoc_ctor_CeUtils_exit; // Success

    // Unwind on error.
__nvoc_ctor_CeUtils_fail__init:
    __nvoc_dtor_Object((Dynamic *)&pCeUtils->__nvoc_base_Object);
__nvoc_ctor_CeUtils_fail_Object:
__nvoc_ctor_CeUtils_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CeUtils_1(CeUtils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CeUtils_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CeUtils(CeUtils *pThis) {
    __nvoc_init_funcTable_CeUtils_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CeUtils(CeUtils *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_CeUtils = pThis;    // (ceutils) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CeUtils.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CeUtils;    // (ceutils) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CeUtils(pThis);
}

NV_STATUS __nvoc_objCreate_CeUtils(CeUtils **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct OBJGPU *pGpu, KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS *pAllocParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CeUtils *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CeUtils));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CeUtils));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__CeUtils(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_CeUtils(__nvoc_pThis, pGpu, pKernelMIGGPUInstance, pAllocParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CeUtils_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CeUtils_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CeUtils));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CeUtils(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct OBJGPU *pGpu = va_arg(__nvoc_args, struct OBJGPU *);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance = va_arg(__nvoc_args, KERNEL_MIG_GPU_INSTANCE *);
    NV0050_ALLOCATION_PARAMETERS *pAllocParams = va_arg(__nvoc_args, NV0050_ALLOCATION_PARAMETERS *);

    return __nvoc_objCreate_CeUtils((CeUtils **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pGpu, pKernelMIGGPUInstance, pAllocParams);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__2eb528 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtilsApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for CeUtilsApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__CeUtilsApi(CeUtilsApi*);
void __nvoc_init_funcTable_CeUtilsApi(CeUtilsApi*);
NV_STATUS __nvoc_ctor_CeUtilsApi(CeUtilsApi*, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_CeUtilsApi(CeUtilsApi*);
void __nvoc_dtor_CeUtilsApi(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CeUtilsApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CeUtilsApi;

// Down-thunk(s) to bridge CeUtilsApi methods from ancestors (if any)
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

// Up-thunk(s) to bridge CeUtilsApi methods to ancestors (if any)

// Class-specific details for CeUtilsApi
const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtilsApi = 
{
    .classInfo.size =               sizeof(CeUtilsApi),
    .classInfo.classId =            classId(CeUtilsApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CeUtilsApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_CeUtilsApi,
    .pCastInfo =          &__nvoc_castinfo__CeUtilsApi,
    .pExportInfo =        &__nvoc_export_info__CeUtilsApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000104u)
static NV_STATUS ceutilsapiCtrlCmdMemset__EXPORT(void *pCeUtilsApi, void *pParams) {
    return ceutilsapiCtrlCmdMemset_IMPL(pCeUtilsApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000104u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000104u)
static NV_STATUS ceutilsapiCtrlCmdMemcopy__EXPORT(void *pCeUtilsApi, void *pParams) {
    return ceutilsapiCtrlCmdMemcopy_IMPL(pCeUtilsApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000104u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000104u)
static NV_STATUS ceutilsapiCtrlCmdCheckProgress__EXPORT(void *pCeUtilsApi, void *pParams) {
    return ceutilsapiCtrlCmdCheckProgress_IMPL(pCeUtilsApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000104u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_CeUtilsApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ceutilsapiCtrlCmdMemset__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*flags=*/      0x104u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x500101u,
        /*paramSize=*/  sizeof(NV0050_CTRL_MEMSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_CeUtilsApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ceutilsapiCtrlCmdMemset"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ceutilsapiCtrlCmdMemcopy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*flags=*/      0x104u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x500102u,
        /*paramSize=*/  sizeof(NV0050_CTRL_MEMCOPY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_CeUtilsApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ceutilsapiCtrlCmdMemcopy"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ceutilsapiCtrlCmdCheckProgress__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*flags=*/      0x104u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x500103u,
        /*paramSize=*/  sizeof(NV0050_CTRL_CHECK_PROGRESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_CeUtilsApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ceutilsapiCtrlCmdCheckProgress"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CeUtilsApi __nvoc_metadata__CeUtilsApi = {
    .rtti.pClassDef = &__nvoc_class_def_CeUtilsApi,    // (ceutilsapi) this
    .rtti.dtor      = &__nvoc_dtor_CeUtilsApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

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
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
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
const struct NVOC_CASTINFO __nvoc_castinfo__CeUtilsApi = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__CeUtilsApi.rtti,    // [0]: (ceutilsapi) this
        &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CeUtilsApi = 
{
    .numEntries=     3,
    .pExportEntries= __nvoc_exported_method_def_CeUtilsApi
};


// Destruct CeUtilsApi object.
void __nvoc_ceutilsapiDestruct(CeUtilsApi*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_CeUtilsApi(Dynamic* pThis) {

    CeUtilsApi *__nvoc_this = (CeUtilsApi *) pThis;

// Call destructor.
    __nvoc_ceutilsapiDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_CeUtilsApi(CeUtilsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct CeUtilsApi object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_CeUtilsApi(CeUtilsApi *pCeUtilsApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pCeUtilsApi->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtilsApi_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_CeUtilsApi(pCeUtilsApi);

    // Call the constructor for this class.
    status = __nvoc_ceutilsapiConstruct(pCeUtilsApi, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtilsApi_fail__init;
    goto __nvoc_ctor_CeUtilsApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_CeUtilsApi_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pCeUtilsApi->__nvoc_base_GpuResource);
__nvoc_ctor_CeUtilsApi_fail_GpuResource:
__nvoc_ctor_CeUtilsApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CeUtilsApi_1(CeUtilsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CeUtilsApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CeUtilsApi(CeUtilsApi *pThis) {
    __nvoc_init_funcTable_CeUtilsApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CeUtilsApi(CeUtilsApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_CeUtilsApi = pThis;    // (ceutilsapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__CeUtilsApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__CeUtilsApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CeUtilsApi;    // (ceutilsapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CeUtilsApi(pThis);
}

NV_STATUS __nvoc_objCreate_CeUtilsApi(CeUtilsApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CeUtilsApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CeUtilsApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CeUtilsApi));

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
    __nvoc_init__CeUtilsApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_CeUtilsApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CeUtilsApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CeUtilsApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CeUtilsApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CeUtilsApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_CeUtilsApi((CeUtilsApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

