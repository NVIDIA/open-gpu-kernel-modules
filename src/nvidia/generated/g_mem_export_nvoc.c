#define NVOC_MEM_EXPORT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_export_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xe7ac53 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryExport;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_MemoryExport(MemoryExport*);
void __nvoc_init_funcTable_MemoryExport(MemoryExport*);
NV_STATUS __nvoc_ctor_MemoryExport(MemoryExport*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryExport(MemoryExport*);
void __nvoc_dtor_MemoryExport(MemoryExport*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryExport;

static const struct NVOC_RTTI __nvoc_rtti_MemoryExport_MemoryExport = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryExport,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryExport,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryExport_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryExport_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryExport_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryExport_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryExport = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryExport_MemoryExport,
        &__nvoc_rtti_MemoryExport_RmResource,
        &__nvoc_rtti_MemoryExport_RmResourceCommon,
        &__nvoc_rtti_MemoryExport_RsResource,
        &__nvoc_rtti_MemoryExport_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryExport = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryExport),
        /*classId=*/            classId(MemoryExport),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryExport",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryExport,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryExport,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryExport
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryExport[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryexportCtrlExportMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xe00101u,
        /*paramSize=*/  sizeof(NV00E0_CTRL_EXPORT_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryExport.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryexportCtrlExportMem"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryexportCtrlImportMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xe00102u,
        /*paramSize=*/  sizeof(NV00E0_CTRL_IMPORT_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryExport.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryexportCtrlImportMem"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryexportCtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xe00103u,
        /*paramSize=*/  sizeof(NV00E0_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryExport.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryexportCtrlGetInfo"
#endif
    },

};

// 2 down-thunk(s) defined to bridge methods in MemoryExport from superclasses

// memoryexportCanCopy: virtual override (res) base (rmres)
static NvBool __nvoc_down_thunk_MemoryExport_resCanCopy(struct RsResource *pMemoryExport) {
    return memoryexportCanCopy((struct MemoryExport *)(((unsigned char *) pMemoryExport) - __nvoc_rtti_MemoryExport_RsResource.offset));
}

// memoryexportControl: virtual override (res) base (rmres)
static NV_STATUS __nvoc_down_thunk_MemoryExport_resControl(struct RsResource *pMemoryExport, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryexportControl((struct MemoryExport *)(((unsigned char *) pMemoryExport) - __nvoc_rtti_MemoryExport_RsResource.offset), pCallContext, pParams);
}


// 19 up-thunk(s) defined to bridge methods in MemoryExport to superclasses

// memoryexportAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_memoryexportAccessCallback(struct MemoryExport *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// memoryexportShareCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_memoryexportShareCallback(struct MemoryExport *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryexportGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryexportGetMemInterMapParams(struct MemoryExport *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pParams);
}

// memoryexportCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryexportCheckMemInterUnmap(struct MemoryExport *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryExport_RmResource.offset), bSubdeviceHandleProvided);
}

// memoryexportGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryexportGetMemoryMappingDescriptor(struct MemoryExport *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryExport_RmResource.offset), ppMemDesc);
}

// memoryexportControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryexportControlSerialization_Prologue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pCallContext, pParams);
}

// memoryexportControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_memoryexportControlSerialization_Epilogue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pCallContext, pParams);
}

// memoryexportControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryexportControl_Prologue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pCallContext, pParams);
}

// memoryexportControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_memoryexportControl_Epilogue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RmResource.offset), pCallContext, pParams);
}

// memoryexportIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryexportIsDuplicate(struct MemoryExport *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), hMemory, pDuplicate);
}

// memoryexportPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_memoryexportPreDestruct(struct MemoryExport *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset));
}

// memoryexportControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryexportControlFilter(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), pCallContext, pParams);
}

// memoryexportMap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryexportMap(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// memoryexportUnmap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryexportUnmap(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), pCallContext, pCpuMapping);
}

// memoryexportIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_memoryexportIsPartialUnmapSupported(struct MemoryExport *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset));
}

// memoryexportMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryexportMapTo(struct MemoryExport *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), pParams);
}

// memoryexportUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryexportUnmapFrom(struct MemoryExport *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), pParams);
}

// memoryexportGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_memoryexportGetRefCount(struct MemoryExport *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset));
}

// memoryexportAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_memoryexportAddAdditionalDependants(struct RsClient *pClient, struct MemoryExport *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryExport_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryExport = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryExport
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MemoryExport(MemoryExport *pThis) {
    __nvoc_memoryexportDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryExport(MemoryExport *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryExport(MemoryExport *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryExport_fail_RmResource;
    __nvoc_init_dataField_MemoryExport(pThis);

    status = __nvoc_memoryexportConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryExport_fail__init;
    goto __nvoc_ctor_MemoryExport_exit; // Success

__nvoc_ctor_MemoryExport_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MemoryExport_fail_RmResource:
__nvoc_ctor_MemoryExport_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryExport_1(MemoryExport *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memoryexportCanCopy -- virtual override (res) base (rmres)
    pThis->__memoryexportCanCopy__ = &memoryexportCanCopy_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryExport_resCanCopy;

    // memoryexportControl -- virtual override (res) base (rmres)
    pThis->__memoryexportControl__ = &memoryexportControl_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__ = &__nvoc_down_thunk_MemoryExport_resControl;

    // memoryexportCtrlExportMem -- exported (id=0xe00101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memoryexportCtrlExportMem__ = &memoryexportCtrlExportMem_IMPL;
#endif

    // memoryexportCtrlImportMem -- exported (id=0xe00102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memoryexportCtrlImportMem__ = &memoryexportCtrlImportMem_IMPL;
#endif

    // memoryexportCtrlGetInfo -- exported (id=0xe00103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memoryexportCtrlGetInfo__ = &memoryexportCtrlGetInfo_IMPL;
#endif

    // memoryexportAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportAccessCallback__ = &__nvoc_up_thunk_RmResource_memoryexportAccessCallback;

    // memoryexportShareCallback -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportShareCallback__ = &__nvoc_up_thunk_RmResource_memoryexportShareCallback;

    // memoryexportGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_memoryexportGetMemInterMapParams;

    // memoryexportCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_memoryexportCheckMemInterUnmap;

    // memoryexportGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_memoryexportGetMemoryMappingDescriptor;

    // memoryexportControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryexportControlSerialization_Prologue;

    // memoryexportControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryexportControlSerialization_Epilogue;

    // memoryexportControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportControl_Prologue__ = &__nvoc_up_thunk_RmResource_memoryexportControl_Prologue;

    // memoryexportControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__memoryexportControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryexportControl_Epilogue;

    // memoryexportIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__memoryexportIsDuplicate__ = &__nvoc_up_thunk_RsResource_memoryexportIsDuplicate;

    // memoryexportPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__memoryexportPreDestruct__ = &__nvoc_up_thunk_RsResource_memoryexportPreDestruct;

    // memoryexportControlFilter -- virtual inherited (res) base (rmres)
    pThis->__memoryexportControlFilter__ = &__nvoc_up_thunk_RsResource_memoryexportControlFilter;

    // memoryexportMap -- virtual inherited (res) base (rmres)
    pThis->__memoryexportMap__ = &__nvoc_up_thunk_RsResource_memoryexportMap;

    // memoryexportUnmap -- virtual inherited (res) base (rmres)
    pThis->__memoryexportUnmap__ = &__nvoc_up_thunk_RsResource_memoryexportUnmap;

    // memoryexportIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__memoryexportIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryexportIsPartialUnmapSupported;

    // memoryexportMapTo -- virtual inherited (res) base (rmres)
    pThis->__memoryexportMapTo__ = &__nvoc_up_thunk_RsResource_memoryexportMapTo;

    // memoryexportUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__memoryexportUnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryexportUnmapFrom;

    // memoryexportGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__memoryexportGetRefCount__ = &__nvoc_up_thunk_RsResource_memoryexportGetRefCount;

    // memoryexportAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__memoryexportAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryexportAddAdditionalDependants;
} // End __nvoc_init_funcTable_MemoryExport_1 with approximately 26 basic block(s).


// Initialize vtable(s) for 24 virtual method(s).
void __nvoc_init_funcTable_MemoryExport(MemoryExport *pThis) {

    // Initialize vtable(s) with 24 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryExport_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_MemoryExport(MemoryExport *pThis) {
    pThis->__nvoc_pbase_MemoryExport = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_MemoryExport(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryExport(MemoryExport **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryExport *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryExport), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryExport));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryExport);

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

    __nvoc_init_MemoryExport(pThis);
    status = __nvoc_ctor_MemoryExport(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryExport_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryExport_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryExport));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryExport(MemoryExport **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryExport(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

