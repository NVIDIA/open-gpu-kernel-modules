#define NVOC_KERNEL_CTXSHARE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ctxshare_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5ae2fe = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShare;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_KernelCtxShare(KernelCtxShare*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCtxShare(KernelCtxShare*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelCtxShare(KernelCtxShare*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelCtxShare(KernelCtxShare*, RmHalspecOwner* );
void __nvoc_dtor_KernelCtxShare(KernelCtxShare*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCtxShare;

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShare_KernelCtxShare = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCtxShare,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCtxShare,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShare_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShare, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShare_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShare, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCtxShare = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCtxShare_KernelCtxShare,
        &__nvoc_rtti_KernelCtxShare_RsShared,
        &__nvoc_rtti_KernelCtxShare_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCtxShare,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCtxShare
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCtxShare = 
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

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_KernelCtxShare(KernelCtxShare *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCtxShare = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCtxShare);

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

    __nvoc_init_KernelCtxShare(pThis, pRmhalspecowner);
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
char __nvoc_class_id_uniqueness_check_0x1f9af1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShareApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_KernelCtxShareApi(KernelCtxShareApi*);
void __nvoc_init_funcTable_KernelCtxShareApi(KernelCtxShareApi*);
NV_STATUS __nvoc_ctor_KernelCtxShareApi(KernelCtxShareApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelCtxShareApi(KernelCtxShareApi*);
void __nvoc_dtor_KernelCtxShareApi(KernelCtxShareApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCtxShareApi;

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShareApi_KernelCtxShareApi = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCtxShareApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCtxShareApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShareApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShareApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShareApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShareApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCtxShareApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCtxShareApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCtxShareApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCtxShareApi_KernelCtxShareApi,
        &__nvoc_rtti_KernelCtxShareApi_GpuResource,
        &__nvoc_rtti_KernelCtxShareApi_RmResource,
        &__nvoc_rtti_KernelCtxShareApi_RmResourceCommon,
        &__nvoc_rtti_KernelCtxShareApi_RsResource,
        &__nvoc_rtti_KernelCtxShareApi_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCtxShareApi,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCtxShareApi
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

// 1 down-thunk(s) defined to bridge methods in KernelCtxShareApi from superclasses

// kctxshareapiCanCopy: virtual override (res) base (gpures)
static NvBool __nvoc_down_thunk_KernelCtxShareApi_resCanCopy(struct RsResource *pKernelCtxShareApi) {
    return kctxshareapiCanCopy((struct KernelCtxShareApi *)(((unsigned char *) pKernelCtxShareApi) - __nvoc_rtti_KernelCtxShareApi_RsResource.offset));
}


// 24 up-thunk(s) defined to bridge methods in KernelCtxShareApi to superclasses

// kctxshareapiControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiControl(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), pCallContext, pParams);
}

// kctxshareapiMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiMap(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// kctxshareapiUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiUnmap(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), pCallContext, pCpuMapping);
}

// kctxshareapiShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_kctxshareapiShareCallback(struct KernelCtxShareApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// kctxshareapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiGetRegBaseOffsetAndSize(struct KernelCtxShareApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), pGpu, pOffset, pSize);
}

// kctxshareapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiGetMapAddrSpace(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// kctxshareapiInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_kctxshareapiInternalControlForward(struct KernelCtxShareApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset), command, pParams, size);
}

// kctxshareapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_kctxshareapiGetInternalObjectHandle(struct KernelCtxShareApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCtxShareApi_GpuResource.offset));
}

// kctxshareapiAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_kctxshareapiAccessCallback(struct KernelCtxShareApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// kctxshareapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiGetMemInterMapParams(struct KernelCtxShareApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), pParams);
}

// kctxshareapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiCheckMemInterUnmap(struct KernelCtxShareApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), bSubdeviceHandleProvided);
}

// kctxshareapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiGetMemoryMappingDescriptor(struct KernelCtxShareApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), ppMemDesc);
}

// kctxshareapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Prologue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), pCallContext, pParams);
}

// kctxshareapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Epilogue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), pCallContext, pParams);
}

// kctxshareapiControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_kctxshareapiControl_Prologue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), pCallContext, pParams);
}

// kctxshareapiControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_kctxshareapiControl_Epilogue(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RmResource.offset), pCallContext, pParams);
}

// kctxshareapiIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiIsDuplicate(struct KernelCtxShareApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset), hMemory, pDuplicate);
}

// kctxshareapiPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_kctxshareapiPreDestruct(struct KernelCtxShareApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset));
}

// kctxshareapiControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiControlFilter(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset), pCallContext, pParams);
}

// kctxshareapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_kctxshareapiIsPartialUnmapSupported(struct KernelCtxShareApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset));
}

// kctxshareapiMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiMapTo(struct KernelCtxShareApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset), pParams);
}

// kctxshareapiUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_kctxshareapiUnmapFrom(struct KernelCtxShareApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset), pParams);
}

// kctxshareapiGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_kctxshareapiGetRefCount(struct KernelCtxShareApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset));
}

// kctxshareapiAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_kctxshareapiAddAdditionalDependants(struct RsClient *pClient, struct KernelCtxShareApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCtxShareApi_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCtxShareApi = 
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

    // kctxshareapiCanCopy -- virtual override (res) base (gpures)
    pThis->__kctxshareapiCanCopy__ = &kctxshareapiCanCopy_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_KernelCtxShareApi_resCanCopy;

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

    // kctxshareapiControl -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiControl__ = &__nvoc_up_thunk_GpuResource_kctxshareapiControl;

    // kctxshareapiMap -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiMap__ = &__nvoc_up_thunk_GpuResource_kctxshareapiMap;

    // kctxshareapiUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiUnmap__ = &__nvoc_up_thunk_GpuResource_kctxshareapiUnmap;

    // kctxshareapiShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiShareCallback__ = &__nvoc_up_thunk_GpuResource_kctxshareapiShareCallback;

    // kctxshareapiGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kctxshareapiGetRegBaseOffsetAndSize;

    // kctxshareapiGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kctxshareapiGetMapAddrSpace;

    // kctxshareapiInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kctxshareapiInternalControlForward;

    // kctxshareapiGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__kctxshareapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kctxshareapiGetInternalObjectHandle;

    // kctxshareapiAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiAccessCallback__ = &__nvoc_up_thunk_RmResource_kctxshareapiAccessCallback;

    // kctxshareapiGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kctxshareapiGetMemInterMapParams;

    // kctxshareapiCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kctxshareapiCheckMemInterUnmap;

    // kctxshareapiGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kctxshareapiGetMemoryMappingDescriptor;

    // kctxshareapiControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Prologue;

    // kctxshareapiControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControlSerialization_Epilogue;

    // kctxshareapiControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControl_Prologue;

    // kctxshareapiControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__kctxshareapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kctxshareapiControl_Epilogue;

    // kctxshareapiIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_kctxshareapiIsDuplicate;

    // kctxshareapiPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiPreDestruct__ = &__nvoc_up_thunk_RsResource_kctxshareapiPreDestruct;

    // kctxshareapiControlFilter -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiControlFilter__ = &__nvoc_up_thunk_RsResource_kctxshareapiControlFilter;

    // kctxshareapiIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__kctxshareapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kctxshareapiIsPartialUnmapSupported;

    // kctxshareapiMapTo -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiMapTo__ = &__nvoc_up_thunk_RsResource_kctxshareapiMapTo;

    // kctxshareapiUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_kctxshareapiUnmapFrom;

    // kctxshareapiGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiGetRefCount__ = &__nvoc_up_thunk_RsResource_kctxshareapiGetRefCount;

    // kctxshareapiAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__kctxshareapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kctxshareapiAddAdditionalDependants;
} // End __nvoc_init_funcTable_KernelCtxShareApi_1 with approximately 29 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_KernelCtxShareApi(KernelCtxShareApi *pThis) {

    // Initialize vtable(s) with 28 per-object function pointer(s).
    __nvoc_init_funcTable_KernelCtxShareApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_KernelCtxShareApi(KernelCtxShareApi *pThis) {
    pThis->__nvoc_pbase_KernelCtxShareApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCtxShareApi);

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

    __nvoc_init_KernelCtxShareApi(pThis);
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

