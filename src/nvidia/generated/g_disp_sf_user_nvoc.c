#define NVOC_DISP_SF_USER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_disp_sf_user_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xba7439 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSfUser;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_DispSfUser(DispSfUser*);
void __nvoc_init_funcTable_DispSfUser(DispSfUser*);
NV_STATUS __nvoc_ctor_DispSfUser(DispSfUser*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispSfUser(DispSfUser*);
void __nvoc_dtor_DispSfUser(DispSfUser*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispSfUser;

static const struct NVOC_RTTI __nvoc_rtti_DispSfUser_DispSfUser = {
    /*pClassDef=*/          &__nvoc_class_def_DispSfUser,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispSfUser,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispSfUser_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSfUser, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSfUser_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSfUser, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSfUser_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSfUser, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSfUser_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSfUser, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSfUser_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSfUser, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispSfUser = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_DispSfUser_DispSfUser,
        &__nvoc_rtti_DispSfUser_GpuResource,
        &__nvoc_rtti_DispSfUser_RmResource,
        &__nvoc_rtti_DispSfUser_RmResourceCommon,
        &__nvoc_rtti_DispSfUser_RsResource,
        &__nvoc_rtti_DispSfUser_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispSfUser = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispSfUser),
        /*classId=*/            classId(DispSfUser),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispSfUser",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispSfUser,
    /*pCastInfo=*/          &__nvoc_castinfo_DispSfUser,
    /*pExportInfo=*/        &__nvoc_export_info_DispSfUser
};

// 1 down-thunk(s) defined to bridge methods in DispSfUser from superclasses

// dispsfGetRegBaseOffsetAndSize: virtual override (gpures) base (gpures)
static NV_STATUS __nvoc_down_thunk_DispSfUser_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispSfUser, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispsfGetRegBaseOffsetAndSize((struct DispSfUser *)(((unsigned char *) pDispSfUser) - __nvoc_rtti_DispSfUser_GpuResource.offset), pGpu, pOffset, pSize);
}


// 24 up-thunk(s) defined to bridge methods in DispSfUser to superclasses

// dispsfControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dispsfControl(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset), pCallContext, pParams);
}

// dispsfMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dispsfMap(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// dispsfUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dispsfUnmap(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset), pCallContext, pCpuMapping);
}

// dispsfShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_dispsfShareCallback(struct DispSfUser *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// dispsfGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dispsfGetMapAddrSpace(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// dispsfInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dispsfInternalControlForward(struct DispSfUser *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset), command, pParams, size);
}

// dispsfGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_dispsfGetInternalObjectHandle(struct DispSfUser *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DispSfUser_GpuResource.offset));
}

// dispsfAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_dispsfAccessCallback(struct DispSfUser *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// dispsfGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dispsfGetMemInterMapParams(struct DispSfUser *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DispSfUser_RmResource.offset), pParams);
}

// dispsfCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dispsfCheckMemInterUnmap(struct DispSfUser *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DispSfUser_RmResource.offset), bSubdeviceHandleProvided);
}

// dispsfGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dispsfGetMemoryMappingDescriptor(struct DispSfUser *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DispSfUser_RmResource.offset), ppMemDesc);
}

// dispsfControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dispsfControlSerialization_Prologue(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RmResource.offset), pCallContext, pParams);
}

// dispsfControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_dispsfControlSerialization_Epilogue(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RmResource.offset), pCallContext, pParams);
}

// dispsfControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dispsfControl_Prologue(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RmResource.offset), pCallContext, pParams);
}

// dispsfControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_dispsfControl_Epilogue(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RmResource.offset), pCallContext, pParams);
}

// dispsfCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_dispsfCanCopy(struct DispSfUser *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset));
}

// dispsfIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dispsfIsDuplicate(struct DispSfUser *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset), hMemory, pDuplicate);
}

// dispsfPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_dispsfPreDestruct(struct DispSfUser *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset));
}

// dispsfControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dispsfControlFilter(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset), pCallContext, pParams);
}

// dispsfIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_dispsfIsPartialUnmapSupported(struct DispSfUser *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset));
}

// dispsfMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dispsfMapTo(struct DispSfUser *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset), pParams);
}

// dispsfUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dispsfUnmapFrom(struct DispSfUser *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset), pParams);
}

// dispsfGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_dispsfGetRefCount(struct DispSfUser *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset));
}

// dispsfAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_dispsfAddAdditionalDependants(struct RsClient *pClient, struct DispSfUser *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DispSfUser_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_DispSfUser = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_DispSfUser(DispSfUser *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispSfUser(DispSfUser *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispSfUser(DispSfUser *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSfUser_fail_GpuResource;
    __nvoc_init_dataField_DispSfUser(pThis);

    status = __nvoc_dispsfConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSfUser_fail__init;
    goto __nvoc_ctor_DispSfUser_exit; // Success

__nvoc_ctor_DispSfUser_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DispSfUser_fail_GpuResource:
__nvoc_ctor_DispSfUser_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispSfUser_1(DispSfUser *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // dispsfGetRegBaseOffsetAndSize -- virtual override (gpures) base (gpures)
    pThis->__dispsfGetRegBaseOffsetAndSize__ = &dispsfGetRegBaseOffsetAndSize_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispSfUser_gpuresGetRegBaseOffsetAndSize;

    // dispsfControl -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfControl__ = &__nvoc_up_thunk_GpuResource_dispsfControl;

    // dispsfMap -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfMap__ = &__nvoc_up_thunk_GpuResource_dispsfMap;

    // dispsfUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfUnmap__ = &__nvoc_up_thunk_GpuResource_dispsfUnmap;

    // dispsfShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfShareCallback__ = &__nvoc_up_thunk_GpuResource_dispsfShareCallback;

    // dispsfGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispsfGetMapAddrSpace;

    // dispsfInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispsfInternalControlForward;

    // dispsfGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__dispsfGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispsfGetInternalObjectHandle;

    // dispsfAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfAccessCallback__ = &__nvoc_up_thunk_RmResource_dispsfAccessCallback;

    // dispsfGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispsfGetMemInterMapParams;

    // dispsfCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispsfCheckMemInterUnmap;

    // dispsfGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispsfGetMemoryMappingDescriptor;

    // dispsfControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispsfControlSerialization_Prologue;

    // dispsfControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispsfControlSerialization_Epilogue;

    // dispsfControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispsfControl_Prologue;

    // dispsfControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__dispsfControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispsfControl_Epilogue;

    // dispsfCanCopy -- virtual inherited (res) base (gpures)
    pThis->__dispsfCanCopy__ = &__nvoc_up_thunk_RsResource_dispsfCanCopy;

    // dispsfIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__dispsfIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispsfIsDuplicate;

    // dispsfPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__dispsfPreDestruct__ = &__nvoc_up_thunk_RsResource_dispsfPreDestruct;

    // dispsfControlFilter -- virtual inherited (res) base (gpures)
    pThis->__dispsfControlFilter__ = &__nvoc_up_thunk_RsResource_dispsfControlFilter;

    // dispsfIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__dispsfIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispsfIsPartialUnmapSupported;

    // dispsfMapTo -- virtual inherited (res) base (gpures)
    pThis->__dispsfMapTo__ = &__nvoc_up_thunk_RsResource_dispsfMapTo;

    // dispsfUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__dispsfUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispsfUnmapFrom;

    // dispsfGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__dispsfGetRefCount__ = &__nvoc_up_thunk_RsResource_dispsfGetRefCount;

    // dispsfAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__dispsfAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispsfAddAdditionalDependants;
} // End __nvoc_init_funcTable_DispSfUser_1 with approximately 26 basic block(s).


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_DispSfUser(DispSfUser *pThis) {

    // Initialize vtable(s) with 25 per-object function pointer(s).
    __nvoc_init_funcTable_DispSfUser_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_DispSfUser(DispSfUser *pThis) {
    pThis->__nvoc_pbase_DispSfUser = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_DispSfUser(pThis);
}

NV_STATUS __nvoc_objCreate_DispSfUser(DispSfUser **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispSfUser *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispSfUser), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispSfUser));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispSfUser);

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

    __nvoc_init_DispSfUser(pThis);
    status = __nvoc_ctor_DispSfUser(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispSfUser_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispSfUser_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispSfUser));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispSfUser(DispSfUser **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispSfUser(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

