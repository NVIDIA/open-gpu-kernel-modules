#define NVOC_THIRD_PARTY_P2P_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_third_party_p2p_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x34d08b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ThirdPartyP2P;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_ThirdPartyP2P(ThirdPartyP2P*);
void __nvoc_init_funcTable_ThirdPartyP2P(ThirdPartyP2P*);
NV_STATUS __nvoc_ctor_ThirdPartyP2P(ThirdPartyP2P*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ThirdPartyP2P(ThirdPartyP2P*);
void __nvoc_dtor_ThirdPartyP2P(ThirdPartyP2P*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ThirdPartyP2P;

static const struct NVOC_RTTI __nvoc_rtti_ThirdPartyP2P_ThirdPartyP2P = {
    /*pClassDef=*/          &__nvoc_class_def_ThirdPartyP2P,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ThirdPartyP2P,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ThirdPartyP2P_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ThirdPartyP2P_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ThirdPartyP2P_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ThirdPartyP2P_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ThirdPartyP2P_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ThirdPartyP2P = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ThirdPartyP2P_ThirdPartyP2P,
        &__nvoc_rtti_ThirdPartyP2P_GpuResource,
        &__nvoc_rtti_ThirdPartyP2P_RmResource,
        &__nvoc_rtti_ThirdPartyP2P_RmResourceCommon,
        &__nvoc_rtti_ThirdPartyP2P_RsResource,
        &__nvoc_rtti_ThirdPartyP2P_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ThirdPartyP2P = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ThirdPartyP2P),
        /*classId=*/            classId(ThirdPartyP2P),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ThirdPartyP2P",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ThirdPartyP2P,
    /*pCastInfo=*/          &__nvoc_castinfo_ThirdPartyP2P,
    /*pExportInfo=*/        &__nvoc_export_info_ThirdPartyP2P
};

static NvBool __nvoc_thunk_GpuResource_thirdpartyp2pShareCallback(struct ThirdPartyP2P *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_thirdpartyp2pControl(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_thirdpartyp2pUnmap(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_thirdpartyp2pGetMemInterMapParams(struct ThirdPartyP2P *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ThirdPartyP2P_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_thirdpartyp2pGetMemoryMappingDescriptor(struct ThirdPartyP2P *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ThirdPartyP2P_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_GpuResource_thirdpartyp2pGetMapAddrSpace(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_thirdpartyp2pGetInternalObjectHandle(struct ThirdPartyP2P *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_thirdpartyp2pControlFilter(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_thirdpartyp2pAddAdditionalDependants(struct RsClient *pClient, struct ThirdPartyP2P *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_thirdpartyp2pGetRefCount(struct ThirdPartyP2P *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_thirdpartyp2pCheckMemInterUnmap(struct ThirdPartyP2P *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ThirdPartyP2P_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_thirdpartyp2pMapTo(struct ThirdPartyP2P *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_thirdpartyp2pControl_Prologue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_thirdpartyp2pGetRegBaseOffsetAndSize(struct ThirdPartyP2P *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_thirdpartyp2pCanCopy(struct ThirdPartyP2P *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_thirdpartyp2pInternalControlForward(struct ThirdPartyP2P *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_thirdpartyp2pPreDestruct(struct ThirdPartyP2P *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_thirdpartyp2pUnmapFrom(struct ThirdPartyP2P *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_thirdpartyp2pIsDuplicate(struct ThirdPartyP2P *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_thirdpartyp2pControl_Epilogue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_thirdpartyp2pControlLookup(struct ThirdPartyP2P *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_GpuResource_thirdpartyp2pMap(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ThirdPartyP2P_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_thirdpartyp2pAccessCallback(struct ThirdPartyP2P *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ThirdPartyP2P_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ThirdPartyP2P[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0102u,
        /*paramSize=*/  sizeof(NV503C_CTRL_REGISTER_VA_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdRegisterVaSpace"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0103u,
        /*paramSize=*/  sizeof(NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdUnregisterVaSpace"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdRegisterVidmem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0104u,
        /*paramSize=*/  sizeof(NV503C_CTRL_REGISTER_VIDMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdRegisterVidmem"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0105u,
        /*paramSize=*/  sizeof(NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdUnregisterVidmem"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdRegisterPid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0106u,
        /*paramSize=*/  sizeof(NV503C_CTRL_REGISTER_PID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdRegisterPid"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_ThirdPartyP2P = 
{
    /*numEntries=*/     5,
    /*pExportEntries=*/ __nvoc_exported_method_def_ThirdPartyP2P
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ThirdPartyP2P(ThirdPartyP2P *pThis) {
    __nvoc_thirdpartyp2pDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ThirdPartyP2P(ThirdPartyP2P *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ThirdPartyP2P(ThirdPartyP2P *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ThirdPartyP2P_fail_GpuResource;
    __nvoc_init_dataField_ThirdPartyP2P(pThis);

    status = __nvoc_thirdpartyp2pConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ThirdPartyP2P_fail__init;
    goto __nvoc_ctor_ThirdPartyP2P_exit; // Success

__nvoc_ctor_ThirdPartyP2P_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ThirdPartyP2P_fail_GpuResource:
__nvoc_ctor_ThirdPartyP2P_exit:

    return status;
}

static void __nvoc_init_funcTable_ThirdPartyP2P_1(ThirdPartyP2P *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__thirdpartyp2pCtrlCmdRegisterVaSpace__ = &thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__thirdpartyp2pCtrlCmdUnregisterVaSpace__ = &thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__thirdpartyp2pCtrlCmdRegisterVidmem__ = &thirdpartyp2pCtrlCmdRegisterVidmem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__thirdpartyp2pCtrlCmdUnregisterVidmem__ = &thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__thirdpartyp2pCtrlCmdRegisterPid__ = &thirdpartyp2pCtrlCmdRegisterPid_IMPL;
#endif

    pThis->__thirdpartyp2pShareCallback__ = &__nvoc_thunk_GpuResource_thirdpartyp2pShareCallback;

    pThis->__thirdpartyp2pControl__ = &__nvoc_thunk_GpuResource_thirdpartyp2pControl;

    pThis->__thirdpartyp2pUnmap__ = &__nvoc_thunk_GpuResource_thirdpartyp2pUnmap;

    pThis->__thirdpartyp2pGetMemInterMapParams__ = &__nvoc_thunk_RmResource_thirdpartyp2pGetMemInterMapParams;

    pThis->__thirdpartyp2pGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_thirdpartyp2pGetMemoryMappingDescriptor;

    pThis->__thirdpartyp2pGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_thirdpartyp2pGetMapAddrSpace;

    pThis->__thirdpartyp2pGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_thirdpartyp2pGetInternalObjectHandle;

    pThis->__thirdpartyp2pControlFilter__ = &__nvoc_thunk_RsResource_thirdpartyp2pControlFilter;

    pThis->__thirdpartyp2pAddAdditionalDependants__ = &__nvoc_thunk_RsResource_thirdpartyp2pAddAdditionalDependants;

    pThis->__thirdpartyp2pGetRefCount__ = &__nvoc_thunk_RsResource_thirdpartyp2pGetRefCount;

    pThis->__thirdpartyp2pCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_thirdpartyp2pCheckMemInterUnmap;

    pThis->__thirdpartyp2pMapTo__ = &__nvoc_thunk_RsResource_thirdpartyp2pMapTo;

    pThis->__thirdpartyp2pControl_Prologue__ = &__nvoc_thunk_RmResource_thirdpartyp2pControl_Prologue;

    pThis->__thirdpartyp2pGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_thirdpartyp2pGetRegBaseOffsetAndSize;

    pThis->__thirdpartyp2pCanCopy__ = &__nvoc_thunk_RsResource_thirdpartyp2pCanCopy;

    pThis->__thirdpartyp2pInternalControlForward__ = &__nvoc_thunk_GpuResource_thirdpartyp2pInternalControlForward;

    pThis->__thirdpartyp2pPreDestruct__ = &__nvoc_thunk_RsResource_thirdpartyp2pPreDestruct;

    pThis->__thirdpartyp2pUnmapFrom__ = &__nvoc_thunk_RsResource_thirdpartyp2pUnmapFrom;

    pThis->__thirdpartyp2pIsDuplicate__ = &__nvoc_thunk_RsResource_thirdpartyp2pIsDuplicate;

    pThis->__thirdpartyp2pControl_Epilogue__ = &__nvoc_thunk_RmResource_thirdpartyp2pControl_Epilogue;

    pThis->__thirdpartyp2pControlLookup__ = &__nvoc_thunk_RsResource_thirdpartyp2pControlLookup;

    pThis->__thirdpartyp2pMap__ = &__nvoc_thunk_GpuResource_thirdpartyp2pMap;

    pThis->__thirdpartyp2pAccessCallback__ = &__nvoc_thunk_RmResource_thirdpartyp2pAccessCallback;
}

void __nvoc_init_funcTable_ThirdPartyP2P(ThirdPartyP2P *pThis) {
    __nvoc_init_funcTable_ThirdPartyP2P_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_ThirdPartyP2P(ThirdPartyP2P *pThis) {
    pThis->__nvoc_pbase_ThirdPartyP2P = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_ThirdPartyP2P(pThis);
}

NV_STATUS __nvoc_objCreate_ThirdPartyP2P(ThirdPartyP2P **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ThirdPartyP2P *pThis;

    pThis = portMemAllocNonPaged(sizeof(ThirdPartyP2P));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(ThirdPartyP2P));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ThirdPartyP2P);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_ThirdPartyP2P(pThis);
    status = __nvoc_ctor_ThirdPartyP2P(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ThirdPartyP2P_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_ThirdPartyP2P_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ThirdPartyP2P(ThirdPartyP2P **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ThirdPartyP2P(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

