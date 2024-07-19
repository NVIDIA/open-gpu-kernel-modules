#define NVOC_NVENCSESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_nvencsession_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x3434af = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvencSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_NvencSession(NvencSession*, RmHalspecOwner* );
void __nvoc_init_funcTable_NvencSession(NvencSession*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_NvencSession(NvencSession*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NvencSession(NvencSession*, RmHalspecOwner* );
void __nvoc_dtor_NvencSession(NvencSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvencSession;

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_NvencSession = {
    /*pClassDef=*/          &__nvoc_class_def_NvencSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvencSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvencSession = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_NvencSession_NvencSession,
        &__nvoc_rtti_NvencSession_GpuResource,
        &__nvoc_rtti_NvencSession_RmResource,
        &__nvoc_rtti_NvencSession_RmResourceCommon,
        &__nvoc_rtti_NvencSession_RsResource,
        &__nvoc_rtti_NvencSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvencSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvencSession),
        /*classId=*/            classId(NvencSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvencSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvencSession,
    /*pCastInfo=*/          &__nvoc_castinfo_NvencSession,
    /*pExportInfo=*/        &__nvoc_export_info_NvencSession
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_NvencSession[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvencsessionCtrlCmdNvencSwSessionUpdateInfo_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0bc0101u,
        /*paramSize=*/  sizeof(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvencSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvencsessionCtrlCmdNvencSwSessionUpdateInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0bc0102u,
        /*paramSize=*/  sizeof(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvencSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2"
#endif
    },

};

// 25 up-thunk(s) defined to bridge methods in NvencSession to superclasses

// nvencsessionControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvencsessionControl(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, pParams);
}

// nvencsessionMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvencsessionMap(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// nvencsessionUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvencsessionUnmap(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, pCpuMapping);
}

// nvencsessionShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_nvencsessionShareCallback(struct NvencSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// nvencsessionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvencsessionGetRegBaseOffsetAndSize(struct NvencSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pGpu, pOffset, pSize);
}

// nvencsessionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvencsessionGetMapAddrSpace(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// nvencsessionInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvencsessionInternalControlForward(struct NvencSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), command, pParams, size);
}

// nvencsessionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_nvencsessionGetInternalObjectHandle(struct NvencSession *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset));
}

// nvencsessionAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_nvencsessionAccessCallback(struct NvencSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// nvencsessionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvencsessionGetMemInterMapParams(struct NvencSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvencSession_RmResource.offset), pParams);
}

// nvencsessionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvencsessionCheckMemInterUnmap(struct NvencSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvencSession_RmResource.offset), bSubdeviceHandleProvided);
}

// nvencsessionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvencsessionGetMemoryMappingDescriptor(struct NvencSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvencSession_RmResource.offset), ppMemDesc);
}

// nvencsessionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvencsessionControlSerialization_Prologue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

// nvencsessionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_nvencsessionControlSerialization_Epilogue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

// nvencsessionControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvencsessionControl_Prologue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

// nvencsessionControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_nvencsessionControl_Epilogue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

// nvencsessionCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_nvencsessionCanCopy(struct NvencSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

// nvencsessionIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvencsessionIsDuplicate(struct NvencSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset), hMemory, pDuplicate);
}

// nvencsessionPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_nvencsessionPreDestruct(struct NvencSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

// nvencsessionControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvencsessionControlFilter(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pCallContext, pParams);
}

// nvencsessionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_nvencsessionIsPartialUnmapSupported(struct NvencSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

// nvencsessionMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvencsessionMapTo(struct NvencSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pParams);
}

// nvencsessionUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvencsessionUnmapFrom(struct NvencSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pParams);
}

// nvencsessionGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_nvencsessionGetRefCount(struct NvencSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

// nvencsessionAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_nvencsessionAddAdditionalDependants(struct RsClient *pClient, struct NvencSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_NvencSession = 
{
    /*numEntries=*/     2,
    /*pExportEntries=*/ __nvoc_exported_method_def_NvencSession
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_NvencSession(NvencSession *pThis) {
    __nvoc_nvencsessionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvencSession(NvencSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NvencSession(NvencSession *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvencSession_fail_GpuResource;
    __nvoc_init_dataField_NvencSession(pThis, pRmhalspecowner);

    status = __nvoc_nvencsessionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvencSession_fail__init;
    goto __nvoc_ctor_NvencSession_exit; // Success

__nvoc_ctor_NvencSession_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_NvencSession_fail_GpuResource:
__nvoc_ctor_NvencSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvencSession_1(NvencSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // nvencsessionCtrlCmdNvencSwSessionUpdateInfo -- halified (2 hals) exported (id=0xa0bc0101) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__ = &nvencsessionCtrlCmdNvencSwSessionUpdateInfo_46f6a7;
    }
    else
    {
        pThis->__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__ = &nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL;
    }

    // nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2 -- halified (2 hals) exported (id=0xa0bc0102) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__ = &nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_46f6a7;
    }
    else
    {
        pThis->__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__ = &nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL;
    }

    // nvencsessionControl -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionControl__ = &__nvoc_up_thunk_GpuResource_nvencsessionControl;

    // nvencsessionMap -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionMap__ = &__nvoc_up_thunk_GpuResource_nvencsessionMap;

    // nvencsessionUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionUnmap__ = &__nvoc_up_thunk_GpuResource_nvencsessionUnmap;

    // nvencsessionShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionShareCallback__ = &__nvoc_up_thunk_GpuResource_nvencsessionShareCallback;

    // nvencsessionGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_nvencsessionGetRegBaseOffsetAndSize;

    // nvencsessionGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_nvencsessionGetMapAddrSpace;

    // nvencsessionInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_nvencsessionInternalControlForward;

    // nvencsessionGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__nvencsessionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_nvencsessionGetInternalObjectHandle;

    // nvencsessionAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionAccessCallback__ = &__nvoc_up_thunk_RmResource_nvencsessionAccessCallback;

    // nvencsessionGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_nvencsessionGetMemInterMapParams;

    // nvencsessionCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_nvencsessionCheckMemInterUnmap;

    // nvencsessionGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_nvencsessionGetMemoryMappingDescriptor;

    // nvencsessionControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_nvencsessionControlSerialization_Prologue;

    // nvencsessionControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_nvencsessionControlSerialization_Epilogue;

    // nvencsessionControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionControl_Prologue__ = &__nvoc_up_thunk_RmResource_nvencsessionControl_Prologue;

    // nvencsessionControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__nvencsessionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_nvencsessionControl_Epilogue;

    // nvencsessionCanCopy -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionCanCopy__ = &__nvoc_up_thunk_RsResource_nvencsessionCanCopy;

    // nvencsessionIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionIsDuplicate__ = &__nvoc_up_thunk_RsResource_nvencsessionIsDuplicate;

    // nvencsessionPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionPreDestruct__ = &__nvoc_up_thunk_RsResource_nvencsessionPreDestruct;

    // nvencsessionControlFilter -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionControlFilter__ = &__nvoc_up_thunk_RsResource_nvencsessionControlFilter;

    // nvencsessionIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__nvencsessionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_nvencsessionIsPartialUnmapSupported;

    // nvencsessionMapTo -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionMapTo__ = &__nvoc_up_thunk_RsResource_nvencsessionMapTo;

    // nvencsessionUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionUnmapFrom__ = &__nvoc_up_thunk_RsResource_nvencsessionUnmapFrom;

    // nvencsessionGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionGetRefCount__ = &__nvoc_up_thunk_RsResource_nvencsessionGetRefCount;

    // nvencsessionAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__nvencsessionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_nvencsessionAddAdditionalDependants;
} // End __nvoc_init_funcTable_NvencSession_1 with approximately 29 basic block(s).


// Initialize vtable(s) for 27 virtual method(s).
void __nvoc_init_funcTable_NvencSession(NvencSession *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 27 per-object function pointer(s).
    __nvoc_init_funcTable_NvencSession_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_NvencSession(NvencSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_NvencSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_NvencSession(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_NvencSession(NvencSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NvencSession *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvencSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NvencSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvencSession);

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

    __nvoc_init_NvencSession(pThis, pRmhalspecowner);
    status = __nvoc_ctor_NvencSession(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvencSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvencSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvencSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvencSession(NvencSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvencSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

