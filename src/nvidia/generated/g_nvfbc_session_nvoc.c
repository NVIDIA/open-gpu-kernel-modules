#define NVOC_NVFBC_SESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_nvfbc_session_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xcbde75 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvfbcSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_NvfbcSession(NvfbcSession*);
void __nvoc_init_funcTable_NvfbcSession(NvfbcSession*);
NV_STATUS __nvoc_ctor_NvfbcSession(NvfbcSession*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NvfbcSession(NvfbcSession*);
void __nvoc_dtor_NvfbcSession(NvfbcSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvfbcSession;

static const struct NVOC_RTTI __nvoc_rtti_NvfbcSession_NvfbcSession = {
    /*pClassDef=*/          &__nvoc_class_def_NvfbcSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvfbcSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvfbcSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvfbcSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvfbcSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvfbcSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvfbcSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvfbcSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NvfbcSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvfbcSession, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvfbcSession_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvfbcSession, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvfbcSession = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_NvfbcSession_NvfbcSession,
        &__nvoc_rtti_NvfbcSession_GpuResource,
        &__nvoc_rtti_NvfbcSession_RmResource,
        &__nvoc_rtti_NvfbcSession_RmResourceCommon,
        &__nvoc_rtti_NvfbcSession_RsResource,
        &__nvoc_rtti_NvfbcSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvfbcSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvfbcSession),
        /*classId=*/            classId(NvfbcSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvfbcSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvfbcSession,
    /*pCastInfo=*/          &__nvoc_castinfo_NvfbcSession,
    /*pExportInfo=*/        &__nvoc_export_info_NvfbcSession
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_NvfbcSession[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0bd0101u,
        /*paramSize=*/  sizeof(NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvfbcSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo"
#endif
    },

};

// 25 up-thunk(s) defined to bridge methods in NvfbcSession to superclasses

// nvfbcsessionControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvfbcsessionControl(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), pCallContext, pParams);
}

// nvfbcsessionMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvfbcsessionMap(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// nvfbcsessionUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvfbcsessionUnmap(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), pCallContext, pCpuMapping);
}

// nvfbcsessionShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_nvfbcsessionShareCallback(struct NvfbcSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// nvfbcsessionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvfbcsessionGetRegBaseOffsetAndSize(struct NvfbcSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), pGpu, pOffset, pSize);
}

// nvfbcsessionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvfbcsessionGetMapAddrSpace(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// nvfbcsessionInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvfbcsessionInternalControlForward(struct NvfbcSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset), command, pParams, size);
}

// nvfbcsessionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_nvfbcsessionGetInternalObjectHandle(struct NvfbcSession *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvfbcSession_GpuResource.offset));
}

// nvfbcsessionAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_nvfbcsessionAccessCallback(struct NvfbcSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// nvfbcsessionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvfbcsessionGetMemInterMapParams(struct NvfbcSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), pParams);
}

// nvfbcsessionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvfbcsessionCheckMemInterUnmap(struct NvfbcSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), bSubdeviceHandleProvided);
}

// nvfbcsessionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvfbcsessionGetMemoryMappingDescriptor(struct NvfbcSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), ppMemDesc);
}

// nvfbcsessionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvfbcsessionControlSerialization_Prologue(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), pCallContext, pParams);
}

// nvfbcsessionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_nvfbcsessionControlSerialization_Epilogue(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), pCallContext, pParams);
}

// nvfbcsessionControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_nvfbcsessionControl_Prologue(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), pCallContext, pParams);
}

// nvfbcsessionControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_nvfbcsessionControl_Epilogue(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RmResource.offset), pCallContext, pParams);
}

// nvfbcsessionCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_nvfbcsessionCanCopy(struct NvfbcSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset));
}

// nvfbcsessionIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvfbcsessionIsDuplicate(struct NvfbcSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset), hMemory, pDuplicate);
}

// nvfbcsessionPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_nvfbcsessionPreDestruct(struct NvfbcSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset));
}

// nvfbcsessionControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvfbcsessionControlFilter(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset), pCallContext, pParams);
}

// nvfbcsessionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_nvfbcsessionIsPartialUnmapSupported(struct NvfbcSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset));
}

// nvfbcsessionMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvfbcsessionMapTo(struct NvfbcSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset), pParams);
}

// nvfbcsessionUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_nvfbcsessionUnmapFrom(struct NvfbcSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset), pParams);
}

// nvfbcsessionGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_nvfbcsessionGetRefCount(struct NvfbcSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset));
}

// nvfbcsessionAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_nvfbcsessionAddAdditionalDependants(struct RsClient *pClient, struct NvfbcSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvfbcSession_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_NvfbcSession = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_NvfbcSession
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_NvfbcSession(NvfbcSession *pThis) {
    __nvoc_nvfbcsessionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvfbcSession(NvfbcSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NvfbcSession(NvfbcSession *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvfbcSession_fail_GpuResource;
    __nvoc_init_dataField_NvfbcSession(pThis);

    status = __nvoc_nvfbcsessionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvfbcSession_fail__init;
    goto __nvoc_ctor_NvfbcSession_exit; // Success

__nvoc_ctor_NvfbcSession_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_NvfbcSession_fail_GpuResource:
__nvoc_ctor_NvfbcSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvfbcSession_1(NvfbcSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo -- exported (id=0xa0bd0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo__ = &nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_IMPL;
#endif

    // nvfbcsessionControl -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionControl__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionControl;

    // nvfbcsessionMap -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionMap__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionMap;

    // nvfbcsessionUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionUnmap__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionUnmap;

    // nvfbcsessionShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionShareCallback__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionShareCallback;

    // nvfbcsessionGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionGetRegBaseOffsetAndSize;

    // nvfbcsessionGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionGetMapAddrSpace;

    // nvfbcsessionInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionInternalControlForward;

    // nvfbcsessionGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__nvfbcsessionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_nvfbcsessionGetInternalObjectHandle;

    // nvfbcsessionAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionAccessCallback__ = &__nvoc_up_thunk_RmResource_nvfbcsessionAccessCallback;

    // nvfbcsessionGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_nvfbcsessionGetMemInterMapParams;

    // nvfbcsessionCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_nvfbcsessionCheckMemInterUnmap;

    // nvfbcsessionGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_nvfbcsessionGetMemoryMappingDescriptor;

    // nvfbcsessionControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_nvfbcsessionControlSerialization_Prologue;

    // nvfbcsessionControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_nvfbcsessionControlSerialization_Epilogue;

    // nvfbcsessionControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionControl_Prologue__ = &__nvoc_up_thunk_RmResource_nvfbcsessionControl_Prologue;

    // nvfbcsessionControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__nvfbcsessionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_nvfbcsessionControl_Epilogue;

    // nvfbcsessionCanCopy -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionCanCopy__ = &__nvoc_up_thunk_RsResource_nvfbcsessionCanCopy;

    // nvfbcsessionIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionIsDuplicate__ = &__nvoc_up_thunk_RsResource_nvfbcsessionIsDuplicate;

    // nvfbcsessionPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionPreDestruct__ = &__nvoc_up_thunk_RsResource_nvfbcsessionPreDestruct;

    // nvfbcsessionControlFilter -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionControlFilter__ = &__nvoc_up_thunk_RsResource_nvfbcsessionControlFilter;

    // nvfbcsessionIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__nvfbcsessionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_nvfbcsessionIsPartialUnmapSupported;

    // nvfbcsessionMapTo -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionMapTo__ = &__nvoc_up_thunk_RsResource_nvfbcsessionMapTo;

    // nvfbcsessionUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionUnmapFrom__ = &__nvoc_up_thunk_RsResource_nvfbcsessionUnmapFrom;

    // nvfbcsessionGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionGetRefCount__ = &__nvoc_up_thunk_RsResource_nvfbcsessionGetRefCount;

    // nvfbcsessionAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__nvfbcsessionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_nvfbcsessionAddAdditionalDependants;
} // End __nvoc_init_funcTable_NvfbcSession_1 with approximately 26 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_NvfbcSession(NvfbcSession *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_NvfbcSession_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_NvfbcSession(NvfbcSession *pThis) {
    pThis->__nvoc_pbase_NvfbcSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_NvfbcSession(pThis);
}

NV_STATUS __nvoc_objCreate_NvfbcSession(NvfbcSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NvfbcSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvfbcSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NvfbcSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvfbcSession);

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

    __nvoc_init_NvfbcSession(pThis);
    status = __nvoc_ctor_NvfbcSession(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvfbcSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvfbcSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvfbcSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvfbcSession(NvfbcSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvfbcSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

