#define NVOC_MEM_FABRIC_IMPORT_REF_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_fabric_import_ref_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x189bad = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportedRef;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_MemoryFabricImportedRef(MemoryFabricImportedRef*);
void __nvoc_init_funcTable_MemoryFabricImportedRef(MemoryFabricImportedRef*);
NV_STATUS __nvoc_ctor_MemoryFabricImportedRef(MemoryFabricImportedRef*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryFabricImportedRef(MemoryFabricImportedRef*);
void __nvoc_dtor_MemoryFabricImportedRef(MemoryFabricImportedRef*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabricImportedRef;

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportedRef_MemoryFabricImportedRef = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryFabricImportedRef,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryFabricImportedRef,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportedRef_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportedRef_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportedRef_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportedRef_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryFabricImportedRef = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryFabricImportedRef_MemoryFabricImportedRef,
        &__nvoc_rtti_MemoryFabricImportedRef_RmResource,
        &__nvoc_rtti_MemoryFabricImportedRef_RmResourceCommon,
        &__nvoc_rtti_MemoryFabricImportedRef_RsResource,
        &__nvoc_rtti_MemoryFabricImportedRef_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportedRef = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryFabricImportedRef),
        /*classId=*/            classId(MemoryFabricImportedRef),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryFabricImportedRef",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryFabricImportedRef,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryFabricImportedRef,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryFabricImportedRef
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryFabricImportedRef[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricimportedrefCtrlValidate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfb0101u,
        /*paramSize=*/  sizeof(NV00FB_CTRL_VALIDATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabricImportedRef.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricimportedrefCtrlValidate"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in MemoryFabricImportedRef from superclasses

// memoryfabricimportedrefCanCopy: virtual override (res) base (rmres)
static NvBool __nvoc_down_thunk_MemoryFabricImportedRef_resCanCopy(struct RsResource *pMemoryFabricImportedRef) {
    return memoryfabricimportedrefCanCopy((struct MemoryFabricImportedRef *)(((unsigned char *) pMemoryFabricImportedRef) - __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset));
}


// 20 up-thunk(s) defined to bridge methods in MemoryFabricImportedRef to superclasses

// memoryfabricimportedrefAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_memoryfabricimportedrefAccessCallback(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// memoryfabricimportedrefShareCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_memoryfabricimportedrefShareCallback(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryfabricimportedrefGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemInterMapParams(struct MemoryFabricImportedRef *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pParams);
}

// memoryfabricimportedrefCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefCheckMemInterUnmap(struct MemoryFabricImportedRef *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), bSubdeviceHandleProvided);
}

// memoryfabricimportedrefGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemoryMappingDescriptor(struct MemoryFabricImportedRef *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), ppMemDesc);
}

// memoryfabricimportedrefControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Prologue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportedrefControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Epilogue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportedrefControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Prologue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportedrefControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Epilogue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportedrefIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefIsDuplicate(struct MemoryFabricImportedRef *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), hMemory, pDuplicate);
}

// memoryfabricimportedrefPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_memoryfabricimportedrefPreDestruct(struct MemoryFabricImportedRef *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset));
}

// memoryfabricimportedrefControl: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefControl(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pCallContext, pParams);
}

// memoryfabricimportedrefControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefControlFilter(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pCallContext, pParams);
}

// memoryfabricimportedrefMap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefMap(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// memoryfabricimportedrefUnmap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmap(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pCallContext, pCpuMapping);
}

// memoryfabricimportedrefIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_memoryfabricimportedrefIsPartialUnmapSupported(struct MemoryFabricImportedRef *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset));
}

// memoryfabricimportedrefMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefMapTo(struct MemoryFabricImportedRef *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pParams);
}

// memoryfabricimportedrefUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmapFrom(struct MemoryFabricImportedRef *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pParams);
}

// memoryfabricimportedrefGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_memoryfabricimportedrefGetRefCount(struct MemoryFabricImportedRef *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset));
}

// memoryfabricimportedrefAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_memoryfabricimportedrefAddAdditionalDependants(struct RsClient *pClient, struct MemoryFabricImportedRef *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportedRef_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabricImportedRef = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryFabricImportedRef
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MemoryFabricImportedRef(MemoryFabricImportedRef *pThis) {
    __nvoc_memoryfabricimportedrefDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryFabricImportedRef(MemoryFabricImportedRef *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryFabricImportedRef(MemoryFabricImportedRef *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabricImportedRef_fail_RmResource;
    __nvoc_init_dataField_MemoryFabricImportedRef(pThis);

    status = __nvoc_memoryfabricimportedrefConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabricImportedRef_fail__init;
    goto __nvoc_ctor_MemoryFabricImportedRef_exit; // Success

__nvoc_ctor_MemoryFabricImportedRef_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MemoryFabricImportedRef_fail_RmResource:
__nvoc_ctor_MemoryFabricImportedRef_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryFabricImportedRef_1(MemoryFabricImportedRef *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memoryfabricimportedrefCanCopy -- virtual override (res) base (rmres)
    pThis->__memoryfabricimportedrefCanCopy__ = &memoryfabricimportedrefCanCopy_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabricImportedRef_resCanCopy;

    // memoryfabricimportedrefCtrlValidate -- exported (id=0xfb0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memoryfabricimportedrefCtrlValidate__ = &memoryfabricimportedrefCtrlValidate_IMPL;
#endif

    // memoryfabricimportedrefAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefAccessCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefAccessCallback;

    // memoryfabricimportedrefShareCallback -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefShareCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefShareCallback;

    // memoryfabricimportedrefGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemInterMapParams;

    // memoryfabricimportedrefCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefCheckMemInterUnmap;

    // memoryfabricimportedrefGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemoryMappingDescriptor;

    // memoryfabricimportedrefControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Prologue;

    // memoryfabricimportedrefControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Epilogue;

    // memoryfabricimportedrefControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefControl_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Prologue;

    // memoryfabricimportedrefControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryfabricimportedrefControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Epilogue;

    // memoryfabricimportedrefIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefIsDuplicate__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefIsDuplicate;

    // memoryfabricimportedrefPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefPreDestruct__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefPreDestruct;

    // memoryfabricimportedrefControl -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefControl__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefControl;

    // memoryfabricimportedrefControlFilter -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefControlFilter__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefControlFilter;

    // memoryfabricimportedrefMap -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefMap__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefMap;

    // memoryfabricimportedrefUnmap -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefUnmap__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmap;

    // memoryfabricimportedrefIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__memoryfabricimportedrefIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefIsPartialUnmapSupported;

    // memoryfabricimportedrefMapTo -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefMapTo__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefMapTo;

    // memoryfabricimportedrefUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefUnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmapFrom;

    // memoryfabricimportedrefGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefGetRefCount__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefGetRefCount;

    // memoryfabricimportedrefAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__memoryfabricimportedrefAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefAddAdditionalDependants;
} // End __nvoc_init_funcTable_MemoryFabricImportedRef_1 with approximately 23 basic block(s).


// Initialize vtable(s) for 22 virtual method(s).
void __nvoc_init_funcTable_MemoryFabricImportedRef(MemoryFabricImportedRef *pThis) {

    // Initialize vtable(s) with 22 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryFabricImportedRef_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_MemoryFabricImportedRef(MemoryFabricImportedRef *pThis) {
    pThis->__nvoc_pbase_MemoryFabricImportedRef = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_MemoryFabricImportedRef(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryFabricImportedRef(MemoryFabricImportedRef **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryFabricImportedRef *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryFabricImportedRef), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryFabricImportedRef));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryFabricImportedRef);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_MemoryFabricImportedRef(pThis);
    status = __nvoc_ctor_MemoryFabricImportedRef(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryFabricImportedRef_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryFabricImportedRef_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryFabricImportedRef));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportedRef(MemoryFabricImportedRef **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryFabricImportedRef(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

