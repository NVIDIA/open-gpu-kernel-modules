#define NVOC_CE_UTILS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_ce_utils_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x8b8bae = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_CeUtils(CeUtils*);
void __nvoc_init_funcTable_CeUtils(CeUtils*);
NV_STATUS __nvoc_ctor_CeUtils(CeUtils*, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams);
void __nvoc_init_dataField_CeUtils(CeUtils*);
void __nvoc_dtor_CeUtils(CeUtils*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_CeUtils;

static const struct NVOC_RTTI __nvoc_rtti_CeUtils_CeUtils = {
    /*pClassDef=*/          &__nvoc_class_def_CeUtils,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CeUtils,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtils_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtils, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_CeUtils = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_CeUtils_CeUtils,
        &__nvoc_rtti_CeUtils_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CeUtils),
        /*classId=*/            classId(CeUtils),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CeUtils",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CeUtils,
    /*pCastInfo=*/          &__nvoc_castinfo_CeUtils,
    /*pExportInfo=*/        &__nvoc_export_info_CeUtils
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_CeUtils = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CeUtils(CeUtils *pThis) {
    __nvoc_ceutilsDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_CeUtils(CeUtils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_CeUtils(CeUtils *pThis, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CeUtils_fail_Object;
    __nvoc_init_dataField_CeUtils(pThis);

    status = __nvoc_ceutilsConstruct(pThis, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtils_fail__init;
    goto __nvoc_ctor_CeUtils_exit; // Success

__nvoc_ctor_CeUtils_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
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

void __nvoc_init_Object(Object*);
void __nvoc_init_CeUtils(CeUtils *pThis) {
    pThis->__nvoc_pbase_CeUtils = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_CeUtils(pThis);
}

NV_STATUS __nvoc_objCreate_CeUtils(CeUtils **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    CeUtils *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(CeUtils), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(CeUtils));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_CeUtils);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_CeUtils(pThis);
    status = __nvoc_ctor_CeUtils(pThis, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams);
    if (status != NV_OK) goto __nvoc_objCreate_CeUtils_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_CeUtils_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(CeUtils));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_CeUtils(CeUtils **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pGpu = va_arg(args, struct OBJGPU *);
    KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance = va_arg(args, KERNEL_MIG_GPU_INSTANCE *);
    NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams = va_arg(args, NV0050_ALLOCATION_PARAMETERS *);

    status = __nvoc_objCreate_CeUtils(ppThis, pParent, createFlags, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2eb528 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtilsApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_CeUtilsApi(CeUtilsApi*);
void __nvoc_init_funcTable_CeUtilsApi(CeUtilsApi*);
NV_STATUS __nvoc_ctor_CeUtilsApi(CeUtilsApi*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_CeUtilsApi(CeUtilsApi*);
void __nvoc_dtor_CeUtilsApi(CeUtilsApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_CeUtilsApi;

static const struct NVOC_RTTI __nvoc_rtti_CeUtilsApi_CeUtilsApi = {
    /*pClassDef=*/          &__nvoc_class_def_CeUtilsApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CeUtilsApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtilsApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtilsApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtilsApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtilsApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtilsApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtilsApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_CeUtilsApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_CeUtilsApi_CeUtilsApi,
        &__nvoc_rtti_CeUtilsApi_GpuResource,
        &__nvoc_rtti_CeUtilsApi_RmResource,
        &__nvoc_rtti_CeUtilsApi_RmResourceCommon,
        &__nvoc_rtti_CeUtilsApi_RsResource,
        &__nvoc_rtti_CeUtilsApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtilsApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CeUtilsApi),
        /*classId=*/            classId(CeUtilsApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CeUtilsApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CeUtilsApi,
    /*pCastInfo=*/          &__nvoc_castinfo_CeUtilsApi,
    /*pExportInfo=*/        &__nvoc_export_info_CeUtilsApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_CeUtilsApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ceutilsapiCtrlCmdMemset_IMPL,
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
        /*pFunc=*/      (void (*)(void)) ceutilsapiCtrlCmdMemcopy_IMPL,
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
        /*pFunc=*/      (void (*)(void)) ceutilsapiCtrlCmdCheckProgress_IMPL,
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

// 25 up-thunk(s) defined to bridge methods in CeUtilsApi to superclasses

// ceutilsapiControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ceutilsapiControl(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), pCallContext, pParams);
}

// ceutilsapiMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ceutilsapiMap(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// ceutilsapiUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ceutilsapiUnmap(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), pCallContext, pCpuMapping);
}

// ceutilsapiShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_ceutilsapiShareCallback(struct CeUtilsApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// ceutilsapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ceutilsapiGetRegBaseOffsetAndSize(struct CeUtilsApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), pGpu, pOffset, pSize);
}

// ceutilsapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ceutilsapiGetMapAddrSpace(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// ceutilsapiInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ceutilsapiInternalControlForward(struct CeUtilsApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset), command, pParams, size);
}

// ceutilsapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_ceutilsapiGetInternalObjectHandle(struct CeUtilsApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_CeUtilsApi_GpuResource.offset));
}

// ceutilsapiAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_ceutilsapiAccessCallback(struct CeUtilsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// ceutilsapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ceutilsapiGetMemInterMapParams(struct CeUtilsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), pParams);
}

// ceutilsapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ceutilsapiCheckMemInterUnmap(struct CeUtilsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), bSubdeviceHandleProvided);
}

// ceutilsapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ceutilsapiGetMemoryMappingDescriptor(struct CeUtilsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), ppMemDesc);
}

// ceutilsapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ceutilsapiControlSerialization_Prologue(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), pCallContext, pParams);
}

// ceutilsapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_ceutilsapiControlSerialization_Epilogue(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), pCallContext, pParams);
}

// ceutilsapiControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ceutilsapiControl_Prologue(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), pCallContext, pParams);
}

// ceutilsapiControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_ceutilsapiControl_Epilogue(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RmResource.offset), pCallContext, pParams);
}

// ceutilsapiCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_ceutilsapiCanCopy(struct CeUtilsApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset));
}

// ceutilsapiIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ceutilsapiIsDuplicate(struct CeUtilsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset), hMemory, pDuplicate);
}

// ceutilsapiPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_ceutilsapiPreDestruct(struct CeUtilsApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset));
}

// ceutilsapiControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ceutilsapiControlFilter(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset), pCallContext, pParams);
}

// ceutilsapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_ceutilsapiIsPartialUnmapSupported(struct CeUtilsApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset));
}

// ceutilsapiMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ceutilsapiMapTo(struct CeUtilsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset), pParams);
}

// ceutilsapiUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ceutilsapiUnmapFrom(struct CeUtilsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset), pParams);
}

// ceutilsapiGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_ceutilsapiGetRefCount(struct CeUtilsApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset));
}

// ceutilsapiAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_ceutilsapiAddAdditionalDependants(struct RsClient *pClient, struct CeUtilsApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_CeUtilsApi_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_CeUtilsApi = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_CeUtilsApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_CeUtilsApi(CeUtilsApi *pThis) {
    __nvoc_ceutilsapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_CeUtilsApi(CeUtilsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_CeUtilsApi(CeUtilsApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtilsApi_fail_GpuResource;
    __nvoc_init_dataField_CeUtilsApi(pThis);

    status = __nvoc_ceutilsapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtilsApi_fail__init;
    goto __nvoc_ctor_CeUtilsApi_exit; // Success

__nvoc_ctor_CeUtilsApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_CeUtilsApi_fail_GpuResource:
__nvoc_ctor_CeUtilsApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CeUtilsApi_1(CeUtilsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // ceutilsapiCtrlCmdMemset -- exported (id=0x500101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
    pThis->__ceutilsapiCtrlCmdMemset__ = &ceutilsapiCtrlCmdMemset_IMPL;
#endif

    // ceutilsapiCtrlCmdMemcopy -- exported (id=0x500102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
    pThis->__ceutilsapiCtrlCmdMemcopy__ = &ceutilsapiCtrlCmdMemcopy_IMPL;
#endif

    // ceutilsapiCtrlCmdCheckProgress -- exported (id=0x500103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x104u)
    pThis->__ceutilsapiCtrlCmdCheckProgress__ = &ceutilsapiCtrlCmdCheckProgress_IMPL;
#endif

    // ceutilsapiControl -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiControl__ = &__nvoc_up_thunk_GpuResource_ceutilsapiControl;

    // ceutilsapiMap -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiMap__ = &__nvoc_up_thunk_GpuResource_ceutilsapiMap;

    // ceutilsapiUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiUnmap__ = &__nvoc_up_thunk_GpuResource_ceutilsapiUnmap;

    // ceutilsapiShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiShareCallback__ = &__nvoc_up_thunk_GpuResource_ceutilsapiShareCallback;

    // ceutilsapiGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_ceutilsapiGetRegBaseOffsetAndSize;

    // ceutilsapiGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_ceutilsapiGetMapAddrSpace;

    // ceutilsapiInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_ceutilsapiInternalControlForward;

    // ceutilsapiGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__ceutilsapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_ceutilsapiGetInternalObjectHandle;

    // ceutilsapiAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiAccessCallback__ = &__nvoc_up_thunk_RmResource_ceutilsapiAccessCallback;

    // ceutilsapiGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_ceutilsapiGetMemInterMapParams;

    // ceutilsapiCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_ceutilsapiCheckMemInterUnmap;

    // ceutilsapiGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_ceutilsapiGetMemoryMappingDescriptor;

    // ceutilsapiControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_ceutilsapiControlSerialization_Prologue;

    // ceutilsapiControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_ceutilsapiControlSerialization_Epilogue;

    // ceutilsapiControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_ceutilsapiControl_Prologue;

    // ceutilsapiControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__ceutilsapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_ceutilsapiControl_Epilogue;

    // ceutilsapiCanCopy -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiCanCopy__ = &__nvoc_up_thunk_RsResource_ceutilsapiCanCopy;

    // ceutilsapiIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_ceutilsapiIsDuplicate;

    // ceutilsapiPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiPreDestruct__ = &__nvoc_up_thunk_RsResource_ceutilsapiPreDestruct;

    // ceutilsapiControlFilter -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiControlFilter__ = &__nvoc_up_thunk_RsResource_ceutilsapiControlFilter;

    // ceutilsapiIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__ceutilsapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_ceutilsapiIsPartialUnmapSupported;

    // ceutilsapiMapTo -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiMapTo__ = &__nvoc_up_thunk_RsResource_ceutilsapiMapTo;

    // ceutilsapiUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_ceutilsapiUnmapFrom;

    // ceutilsapiGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiGetRefCount__ = &__nvoc_up_thunk_RsResource_ceutilsapiGetRefCount;

    // ceutilsapiAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__ceutilsapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_ceutilsapiAddAdditionalDependants;
} // End __nvoc_init_funcTable_CeUtilsApi_1 with approximately 28 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_CeUtilsApi(CeUtilsApi *pThis) {

    // Initialize vtable(s) with 28 per-object function pointer(s).
    __nvoc_init_funcTable_CeUtilsApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_CeUtilsApi(CeUtilsApi *pThis) {
    pThis->__nvoc_pbase_CeUtilsApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_CeUtilsApi(pThis);
}

NV_STATUS __nvoc_objCreate_CeUtilsApi(CeUtilsApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    CeUtilsApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(CeUtilsApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(CeUtilsApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_CeUtilsApi);

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

    __nvoc_init_CeUtilsApi(pThis);
    status = __nvoc_ctor_CeUtilsApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_CeUtilsApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_CeUtilsApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(CeUtilsApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_CeUtilsApi(CeUtilsApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_CeUtilsApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

