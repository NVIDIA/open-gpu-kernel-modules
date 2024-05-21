#define NVOC_VBLANK_CALLBACK_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_vblank_callback_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4c1997 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VblankCallback;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_VblankCallback(VblankCallback*);
void __nvoc_init_funcTable_VblankCallback(VblankCallback*);
NV_STATUS __nvoc_ctor_VblankCallback(VblankCallback*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VblankCallback(VblankCallback*);
void __nvoc_dtor_VblankCallback(VblankCallback*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VblankCallback;

static const struct NVOC_RTTI __nvoc_rtti_VblankCallback_VblankCallback = {
    /*pClassDef=*/          &__nvoc_class_def_VblankCallback,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VblankCallback,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VblankCallback_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VblankCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VblankCallback_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VblankCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VblankCallback_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VblankCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VblankCallback_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VblankCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VblankCallback_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VblankCallback, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VblankCallback = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_VblankCallback_VblankCallback,
        &__nvoc_rtti_VblankCallback_GpuResource,
        &__nvoc_rtti_VblankCallback_RmResource,
        &__nvoc_rtti_VblankCallback_RmResourceCommon,
        &__nvoc_rtti_VblankCallback_RsResource,
        &__nvoc_rtti_VblankCallback_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VblankCallback = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VblankCallback),
        /*classId=*/            classId(VblankCallback),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VblankCallback",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VblankCallback,
    /*pCastInfo=*/          &__nvoc_castinfo_VblankCallback,
    /*pExportInfo=*/        &__nvoc_export_info_VblankCallback
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VblankCallback[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vblcbCtrlSetVBlankNotification_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90100101u,
        /*paramSize=*/  sizeof(NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VblankCallback.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vblcbCtrlSetVBlankNotification"
#endif
    },

};

// 25 up-thunk(s) defined to bridge methods in VblankCallback to superclasses

// vblcbControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vblcbControl(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), pCallContext, pParams);
}

// vblcbMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vblcbMap(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// vblcbUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vblcbUnmap(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), pCallContext, pCpuMapping);
}

// vblcbShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_vblcbShareCallback(struct VblankCallback *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// vblcbGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vblcbGetRegBaseOffsetAndSize(struct VblankCallback *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), pGpu, pOffset, pSize);
}

// vblcbGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vblcbGetMapAddrSpace(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// vblcbInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vblcbInternalControlForward(struct VblankCallback *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset), command, pParams, size);
}

// vblcbGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_vblcbGetInternalObjectHandle(struct VblankCallback *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VblankCallback_GpuResource.offset));
}

// vblcbAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_vblcbAccessCallback(struct VblankCallback *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// vblcbGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vblcbGetMemInterMapParams(struct VblankCallback *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VblankCallback_RmResource.offset), pParams);
}

// vblcbCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vblcbCheckMemInterUnmap(struct VblankCallback *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VblankCallback_RmResource.offset), bSubdeviceHandleProvided);
}

// vblcbGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vblcbGetMemoryMappingDescriptor(struct VblankCallback *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VblankCallback_RmResource.offset), ppMemDesc);
}

// vblcbControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vblcbControlSerialization_Prologue(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RmResource.offset), pCallContext, pParams);
}

// vblcbControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_vblcbControlSerialization_Epilogue(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RmResource.offset), pCallContext, pParams);
}

// vblcbControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vblcbControl_Prologue(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RmResource.offset), pCallContext, pParams);
}

// vblcbControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_vblcbControl_Epilogue(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RmResource.offset), pCallContext, pParams);
}

// vblcbCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_vblcbCanCopy(struct VblankCallback *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset));
}

// vblcbIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vblcbIsDuplicate(struct VblankCallback *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset), hMemory, pDuplicate);
}

// vblcbPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_vblcbPreDestruct(struct VblankCallback *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset));
}

// vblcbControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vblcbControlFilter(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset), pCallContext, pParams);
}

// vblcbIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_vblcbIsPartialUnmapSupported(struct VblankCallback *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset));
}

// vblcbMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vblcbMapTo(struct VblankCallback *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset), pParams);
}

// vblcbUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vblcbUnmapFrom(struct VblankCallback *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset), pParams);
}

// vblcbGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_vblcbGetRefCount(struct VblankCallback *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset));
}

// vblcbAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_vblcbAddAdditionalDependants(struct RsClient *pClient, struct VblankCallback *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VblankCallback_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_VblankCallback = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_VblankCallback
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_VblankCallback(VblankCallback *pThis) {
    __nvoc_vblcbDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VblankCallback(VblankCallback *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_VblankCallback(VblankCallback *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VblankCallback_fail_GpuResource;
    __nvoc_init_dataField_VblankCallback(pThis);

    status = __nvoc_vblcbConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VblankCallback_fail__init;
    goto __nvoc_ctor_VblankCallback_exit; // Success

__nvoc_ctor_VblankCallback_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_VblankCallback_fail_GpuResource:
__nvoc_ctor_VblankCallback_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VblankCallback_1(VblankCallback *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // vblcbCtrlSetVBlankNotification -- exported (id=0x90100101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__vblcbCtrlSetVBlankNotification__ = &vblcbCtrlSetVBlankNotification_IMPL;
#endif

    // vblcbControl -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbControl__ = &__nvoc_up_thunk_GpuResource_vblcbControl;

    // vblcbMap -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbMap__ = &__nvoc_up_thunk_GpuResource_vblcbMap;

    // vblcbUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbUnmap__ = &__nvoc_up_thunk_GpuResource_vblcbUnmap;

    // vblcbShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbShareCallback__ = &__nvoc_up_thunk_GpuResource_vblcbShareCallback;

    // vblcbGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_vblcbGetRegBaseOffsetAndSize;

    // vblcbGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_vblcbGetMapAddrSpace;

    // vblcbInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbInternalControlForward__ = &__nvoc_up_thunk_GpuResource_vblcbInternalControlForward;

    // vblcbGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__vblcbGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_vblcbGetInternalObjectHandle;

    // vblcbAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbAccessCallback__ = &__nvoc_up_thunk_RmResource_vblcbAccessCallback;

    // vblcbGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_vblcbGetMemInterMapParams;

    // vblcbCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_vblcbCheckMemInterUnmap;

    // vblcbGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_vblcbGetMemoryMappingDescriptor;

    // vblcbControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_vblcbControlSerialization_Prologue;

    // vblcbControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_vblcbControlSerialization_Epilogue;

    // vblcbControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbControl_Prologue__ = &__nvoc_up_thunk_RmResource_vblcbControl_Prologue;

    // vblcbControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__vblcbControl_Epilogue__ = &__nvoc_up_thunk_RmResource_vblcbControl_Epilogue;

    // vblcbCanCopy -- virtual inherited (res) base (gpures)
    pThis->__vblcbCanCopy__ = &__nvoc_up_thunk_RsResource_vblcbCanCopy;

    // vblcbIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__vblcbIsDuplicate__ = &__nvoc_up_thunk_RsResource_vblcbIsDuplicate;

    // vblcbPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__vblcbPreDestruct__ = &__nvoc_up_thunk_RsResource_vblcbPreDestruct;

    // vblcbControlFilter -- virtual inherited (res) base (gpures)
    pThis->__vblcbControlFilter__ = &__nvoc_up_thunk_RsResource_vblcbControlFilter;

    // vblcbIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__vblcbIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_vblcbIsPartialUnmapSupported;

    // vblcbMapTo -- virtual inherited (res) base (gpures)
    pThis->__vblcbMapTo__ = &__nvoc_up_thunk_RsResource_vblcbMapTo;

    // vblcbUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__vblcbUnmapFrom__ = &__nvoc_up_thunk_RsResource_vblcbUnmapFrom;

    // vblcbGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__vblcbGetRefCount__ = &__nvoc_up_thunk_RsResource_vblcbGetRefCount;

    // vblcbAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__vblcbAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_vblcbAddAdditionalDependants;
} // End __nvoc_init_funcTable_VblankCallback_1 with approximately 26 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_VblankCallback(VblankCallback *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_VblankCallback_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_VblankCallback(VblankCallback *pThis) {
    pThis->__nvoc_pbase_VblankCallback = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_VblankCallback(pThis);
}

NV_STATUS __nvoc_objCreate_VblankCallback(VblankCallback **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VblankCallback *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VblankCallback), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VblankCallback));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VblankCallback);

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

    __nvoc_init_VblankCallback(pThis);
    status = __nvoc_ctor_VblankCallback(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VblankCallback_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VblankCallback_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VblankCallback));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VblankCallback(VblankCallback **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VblankCallback(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

