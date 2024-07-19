#define NVOC_SEM_SURF_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_sem_surf_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xeabc69 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SemaphoreSurface;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_SemaphoreSurface(SemaphoreSurface*);
void __nvoc_init_funcTable_SemaphoreSurface(SemaphoreSurface*);
NV_STATUS __nvoc_ctor_SemaphoreSurface(SemaphoreSurface*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_SemaphoreSurface(SemaphoreSurface*);
void __nvoc_dtor_SemaphoreSurface(SemaphoreSurface*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SemaphoreSurface;

static const struct NVOC_RTTI __nvoc_rtti_SemaphoreSurface_SemaphoreSurface = {
    /*pClassDef=*/          &__nvoc_class_def_SemaphoreSurface,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SemaphoreSurface,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SemaphoreSurface_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SemaphoreSurface, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SemaphoreSurface_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SemaphoreSurface, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SemaphoreSurface_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SemaphoreSurface, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_SemaphoreSurface_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SemaphoreSurface, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SemaphoreSurface_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SemaphoreSurface, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SemaphoreSurface = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_SemaphoreSurface_SemaphoreSurface,
        &__nvoc_rtti_SemaphoreSurface_GpuResource,
        &__nvoc_rtti_SemaphoreSurface_RmResource,
        &__nvoc_rtti_SemaphoreSurface_RmResourceCommon,
        &__nvoc_rtti_SemaphoreSurface_RsResource,
        &__nvoc_rtti_SemaphoreSurface_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SemaphoreSurface = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SemaphoreSurface),
        /*classId=*/            classId(SemaphoreSurface),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SemaphoreSurface",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SemaphoreSurface,
    /*pCastInfo=*/          &__nvoc_castinfo_SemaphoreSurface,
    /*pExportInfo=*/        &__nvoc_export_info_SemaphoreSurface
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_SemaphoreSurface[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) semsurfCtrlCmdRefMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xda0001u,
        /*paramSize=*/  sizeof(NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SemaphoreSurface.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "semsurfCtrlCmdRefMemory"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) semsurfCtrlCmdBindChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xda0002u,
        /*paramSize=*/  sizeof(NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SemaphoreSurface.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "semsurfCtrlCmdBindChannel"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400009u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) semsurfCtrlCmdRegisterWaiter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400009u)
        /*flags=*/      0x400009u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xda0003u,
        /*paramSize=*/  sizeof(NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SemaphoreSurface.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "semsurfCtrlCmdRegisterWaiter"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) semsurfCtrlCmdSetValue_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xda0004u,
        /*paramSize=*/  sizeof(NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SemaphoreSurface.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "semsurfCtrlCmdSetValue"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) semsurfCtrlCmdUnregisterWaiter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xda0005u,
        /*paramSize=*/  sizeof(NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SemaphoreSurface.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "semsurfCtrlCmdUnregisterWaiter"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) semsurfCtrlCmdUnbindChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xda0006u,
        /*paramSize=*/  sizeof(NV_SEMAPHORE_SURFACE_CTRL_UNBIND_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SemaphoreSurface.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "semsurfCtrlCmdUnbindChannel"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in SemaphoreSurface from superclasses

// semsurfCanCopy: inline virtual override (res) base (gpures) body
static NvBool __nvoc_down_thunk_SemaphoreSurface_resCanCopy(struct RsResource *pSemSurf) {
    return semsurfCanCopy((struct SemaphoreSurface *)(((unsigned char *) pSemSurf) - __nvoc_rtti_SemaphoreSurface_RsResource.offset));
}


// 24 up-thunk(s) defined to bridge methods in SemaphoreSurface to superclasses

// semsurfControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_semsurfControl(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), pCallContext, pParams);
}

// semsurfMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_semsurfMap(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// semsurfUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_semsurfUnmap(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), pCallContext, pCpuMapping);
}

// semsurfShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_semsurfShareCallback(struct SemaphoreSurface *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// semsurfGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_semsurfGetRegBaseOffsetAndSize(struct SemaphoreSurface *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), pGpu, pOffset, pSize);
}

// semsurfGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_semsurfGetMapAddrSpace(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// semsurfInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_semsurfInternalControlForward(struct SemaphoreSurface *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset), command, pParams, size);
}

// semsurfGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_semsurfGetInternalObjectHandle(struct SemaphoreSurface *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_SemaphoreSurface_GpuResource.offset));
}

// semsurfAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_semsurfAccessCallback(struct SemaphoreSurface *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// semsurfGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_semsurfGetMemInterMapParams(struct SemaphoreSurface *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), pParams);
}

// semsurfCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_semsurfCheckMemInterUnmap(struct SemaphoreSurface *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), bSubdeviceHandleProvided);
}

// semsurfGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_semsurfGetMemoryMappingDescriptor(struct SemaphoreSurface *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), ppMemDesc);
}

// semsurfControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_semsurfControlSerialization_Prologue(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), pCallContext, pParams);
}

// semsurfControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_semsurfControlSerialization_Epilogue(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), pCallContext, pParams);
}

// semsurfControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_semsurfControl_Prologue(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), pCallContext, pParams);
}

// semsurfControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_semsurfControl_Epilogue(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RmResource.offset), pCallContext, pParams);
}

// semsurfIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_semsurfIsDuplicate(struct SemaphoreSurface *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset), hMemory, pDuplicate);
}

// semsurfPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_semsurfPreDestruct(struct SemaphoreSurface *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset));
}

// semsurfControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_semsurfControlFilter(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset), pCallContext, pParams);
}

// semsurfIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_semsurfIsPartialUnmapSupported(struct SemaphoreSurface *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset));
}

// semsurfMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_semsurfMapTo(struct SemaphoreSurface *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset), pParams);
}

// semsurfUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_semsurfUnmapFrom(struct SemaphoreSurface *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset), pParams);
}

// semsurfGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_semsurfGetRefCount(struct SemaphoreSurface *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset));
}

// semsurfAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_semsurfAddAdditionalDependants(struct RsClient *pClient, struct SemaphoreSurface *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SemaphoreSurface_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_SemaphoreSurface = 
{
    /*numEntries=*/     6,
    /*pExportEntries=*/ __nvoc_exported_method_def_SemaphoreSurface
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_SemaphoreSurface(SemaphoreSurface *pThis) {
    __nvoc_semsurfDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SemaphoreSurface(SemaphoreSurface *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_SemaphoreSurface(SemaphoreSurface *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SemaphoreSurface_fail_GpuResource;
    __nvoc_init_dataField_SemaphoreSurface(pThis);

    status = __nvoc_semsurfConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SemaphoreSurface_fail__init;
    goto __nvoc_ctor_SemaphoreSurface_exit; // Success

__nvoc_ctor_SemaphoreSurface_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_SemaphoreSurface_fail_GpuResource:
__nvoc_ctor_SemaphoreSurface_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SemaphoreSurface_1(SemaphoreSurface *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // semsurfCanCopy -- inline virtual override (res) base (gpures) body
    pThis->__semsurfCanCopy__ = &semsurfCanCopy_0c883b;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_SemaphoreSurface_resCanCopy;

    // semsurfCtrlCmdRefMemory -- exported (id=0xda0001)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__semsurfCtrlCmdRefMemory__ = &semsurfCtrlCmdRefMemory_IMPL;
#endif

    // semsurfCtrlCmdBindChannel -- exported (id=0xda0002)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__semsurfCtrlCmdBindChannel__ = &semsurfCtrlCmdBindChannel_IMPL;
#endif

    // semsurfCtrlCmdUnbindChannel -- exported (id=0xda0006)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__semsurfCtrlCmdUnbindChannel__ = &semsurfCtrlCmdUnbindChannel_IMPL;
#endif

    // semsurfCtrlCmdRegisterWaiter -- exported (id=0xda0003)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400009u)
    pThis->__semsurfCtrlCmdRegisterWaiter__ = &semsurfCtrlCmdRegisterWaiter_IMPL;
#endif

    // semsurfCtrlCmdSetValue -- exported (id=0xda0004)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__semsurfCtrlCmdSetValue__ = &semsurfCtrlCmdSetValue_IMPL;
#endif

    // semsurfCtrlCmdUnregisterWaiter -- exported (id=0xda0005)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__semsurfCtrlCmdUnregisterWaiter__ = &semsurfCtrlCmdUnregisterWaiter_IMPL;
#endif

    // semsurfControl -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfControl__ = &__nvoc_up_thunk_GpuResource_semsurfControl;

    // semsurfMap -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfMap__ = &__nvoc_up_thunk_GpuResource_semsurfMap;

    // semsurfUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfUnmap__ = &__nvoc_up_thunk_GpuResource_semsurfUnmap;

    // semsurfShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfShareCallback__ = &__nvoc_up_thunk_GpuResource_semsurfShareCallback;

    // semsurfGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_semsurfGetRegBaseOffsetAndSize;

    // semsurfGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_semsurfGetMapAddrSpace;

    // semsurfInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfInternalControlForward__ = &__nvoc_up_thunk_GpuResource_semsurfInternalControlForward;

    // semsurfGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__semsurfGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_semsurfGetInternalObjectHandle;

    // semsurfAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfAccessCallback__ = &__nvoc_up_thunk_RmResource_semsurfAccessCallback;

    // semsurfGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_semsurfGetMemInterMapParams;

    // semsurfCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_semsurfCheckMemInterUnmap;

    // semsurfGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_semsurfGetMemoryMappingDescriptor;

    // semsurfControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_semsurfControlSerialization_Prologue;

    // semsurfControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_semsurfControlSerialization_Epilogue;

    // semsurfControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfControl_Prologue__ = &__nvoc_up_thunk_RmResource_semsurfControl_Prologue;

    // semsurfControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__semsurfControl_Epilogue__ = &__nvoc_up_thunk_RmResource_semsurfControl_Epilogue;

    // semsurfIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__semsurfIsDuplicate__ = &__nvoc_up_thunk_RsResource_semsurfIsDuplicate;

    // semsurfPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__semsurfPreDestruct__ = &__nvoc_up_thunk_RsResource_semsurfPreDestruct;

    // semsurfControlFilter -- virtual inherited (res) base (gpures)
    pThis->__semsurfControlFilter__ = &__nvoc_up_thunk_RsResource_semsurfControlFilter;

    // semsurfIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__semsurfIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_semsurfIsPartialUnmapSupported;

    // semsurfMapTo -- virtual inherited (res) base (gpures)
    pThis->__semsurfMapTo__ = &__nvoc_up_thunk_RsResource_semsurfMapTo;

    // semsurfUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__semsurfUnmapFrom__ = &__nvoc_up_thunk_RsResource_semsurfUnmapFrom;

    // semsurfGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__semsurfGetRefCount__ = &__nvoc_up_thunk_RsResource_semsurfGetRefCount;

    // semsurfAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__semsurfAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_semsurfAddAdditionalDependants;
} // End __nvoc_init_funcTable_SemaphoreSurface_1 with approximately 32 basic block(s).


// Initialize vtable(s) for 31 virtual method(s).
void __nvoc_init_funcTable_SemaphoreSurface(SemaphoreSurface *pThis) {

    // Initialize vtable(s) with 31 per-object function pointer(s).
    __nvoc_init_funcTable_SemaphoreSurface_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_SemaphoreSurface(SemaphoreSurface *pThis) {
    pThis->__nvoc_pbase_SemaphoreSurface = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_SemaphoreSurface(pThis);
}

NV_STATUS __nvoc_objCreate_SemaphoreSurface(SemaphoreSurface **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SemaphoreSurface *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SemaphoreSurface), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SemaphoreSurface));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SemaphoreSurface);

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

    __nvoc_init_SemaphoreSurface(pThis);
    status = __nvoc_ctor_SemaphoreSurface(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_SemaphoreSurface_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SemaphoreSurface_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SemaphoreSurface));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SemaphoreSurface(SemaphoreSurface **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_SemaphoreSurface(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

