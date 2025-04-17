#define NVOC_RESOURCE_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_resource_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x8ef259 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

// Forward declarations for RmResourceCommon
void __nvoc_init__RmResourceCommon(RmResourceCommon*);
void __nvoc_init_funcTable_RmResourceCommon(RmResourceCommon*);
NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon*);
void __nvoc_init_dataField_RmResourceCommon(RmResourceCommon*);
void __nvoc_dtor_RmResourceCommon(RmResourceCommon*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RmResourceCommon;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RmResourceCommon;

// Down-thunk(s) to bridge RmResourceCommon methods from ancestors (if any)

// Up-thunk(s) to bridge RmResourceCommon methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmResourceCommon),
        /*classId=*/            classId(RmResourceCommon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmResourceCommon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__RmResourceCommon,
    /*pExportInfo=*/        &__nvoc_export_info__RmResourceCommon
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__RmResourceCommon __nvoc_metadata__RmResourceCommon = {
    .rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmResourceCommon,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RmResourceCommon = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__RmResourceCommon.rtti,    // [0]: (rmrescmn) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RmResourceCommon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResourceCommon(RmResourceCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmResourceCommon(RmResourceCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_RmResourceCommon(pThis);

    status = __nvoc_rmrescmnConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_RmResourceCommon_fail__init;
    goto __nvoc_ctor_RmResourceCommon_exit; // Success

__nvoc_ctor_RmResourceCommon_fail__init:
__nvoc_ctor_RmResourceCommon_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmResourceCommon_1(RmResourceCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RmResourceCommon_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RmResourceCommon(RmResourceCommon *pThis) {
    __nvoc_init_funcTable_RmResourceCommon_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RmResourceCommon(RmResourceCommon *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_RmResourceCommon = pThis;    // (rmrescmn) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RmResourceCommon;    // (rmrescmn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RmResourceCommon(pThis);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x03610d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

// Forward declarations for RmResource
void __nvoc_init__RsResource(RsResource*);
void __nvoc_init__RmResourceCommon(RmResourceCommon*);
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init_funcTable_RmResource(RmResource*);
NV_STATUS __nvoc_ctor_RmResource(RmResource*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_RmResource(RmResource*);
void __nvoc_dtor_RmResource(RmResource*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RmResource;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RmResource;

// Down-thunk(s) to bridge RmResource methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge RmResource methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // this
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmResource),
        /*classId=*/            classId(RmResource),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmResource",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RmResource,
    /*pCastInfo=*/          &__nvoc_castinfo__RmResource,
    /*pExportInfo=*/        &__nvoc_export_info__RmResource
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RmResource __nvoc_metadata__RmResource = {
    .rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmResource,
    .rtti.offset    = 0,
    .metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super
    .metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsResource.rtti.offset    = NV_OFFSETOF(RmResource, __nvoc_base_RsResource),
    .metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(RmResource, __nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super
    .metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(RmResource, __nvoc_base_RmResourceCommon),

    .vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RmResource = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__RmResource.rtti,    // [0]: (rmres) this
        &__nvoc_metadata__RmResource.metadata__RsResource.rtti,    // [1]: (res) super
        &__nvoc_metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__RmResource.metadata__RmResourceCommon.rtti,    // [3]: (rmrescmn) super
    }
};

// 6 down-thunk(s) defined to bridge methods in RmResource from superclasses

// rmresAccessCallback: virtual override (res) base (res)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) - NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pInvokingClient, pAllocParams, accessRight);
}

// rmresShareCallback: virtual override (res) base (res)
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) - NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// rmresControlSerialization_Prologue: virtual override (res) base (res)
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) - NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// rmresControlSerialization_Epilogue: virtual override (res) base (res)
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) - NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// rmresControl_Prologue: virtual override (res) base (res)
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) - NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// rmresControl_Epilogue: virtual override (res) base (res)
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) - NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams);
}


// 12 up-thunk(s) defined to bridge methods in RmResource to superclasses

// rmresCanCopy: virtual inherited (res) base (res)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)));
}

// rmresIsDuplicate: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), hMemory, pDuplicate);
}

// rmresPreDestruct: virtual inherited (res) base (res)
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)));
}

// rmresControl: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// rmresControlFilter: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// rmresMap: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// rmresUnmap: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// rmresIsPartialUnmapSupported: inline virtual inherited (res) base (res) body
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)));
}

// rmresMapTo: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pParams);
}

// rmresUnmapFrom: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pParams);
}

// rmresGetRefCount: virtual inherited (res) base (res)
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)));
}

// rmresAddAdditionalDependants: virtual inherited (res) base (res)
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmResource, __nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__RmResource = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsResource(RsResource*);
void __nvoc_dtor_RmResourceCommon(RmResourceCommon*);
void __nvoc_dtor_RmResource(RmResource *pThis) {
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);
    __nvoc_dtor_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmResource(RmResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsResource(RsResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon* );
NV_STATUS __nvoc_ctor_RmResource(RmResource *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsResource(&pThis->__nvoc_base_RsResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmResource_fail_RsResource;
    status = __nvoc_ctor_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
    if (status != NV_OK) goto __nvoc_ctor_RmResource_fail_RmResourceCommon;
    __nvoc_init_dataField_RmResource(pThis);

    status = __nvoc_rmresConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmResource_fail__init;
    goto __nvoc_ctor_RmResource_exit; // Success

__nvoc_ctor_RmResource_fail__init:
    __nvoc_dtor_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
__nvoc_ctor_RmResource_fail_RmResourceCommon:
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);
__nvoc_ctor_RmResource_fail_RsResource:
__nvoc_ctor_RmResource_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmResource_1(RmResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RmResource_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_RmResource(RmResource *pThis) {
    __nvoc_init_funcTable_RmResource_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RmResource(RmResource *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RsResource;    // (res) super
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResourceCommon;    // (rmrescmn) super
    pThis->__nvoc_pbase_RmResource = pThis;    // (rmres) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsResource(&pThis->__nvoc_base_RsResource);
    __nvoc_init__RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__RmResource.metadata__RsResource;    // (res) super
    pThis->__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RmResource;    // (rmres) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RmResource(pThis);
}

NV_STATUS __nvoc_objCreate_RmResource(RmResource **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RmResource *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RmResource), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RmResource));

    pThis->__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__RmResource(pThis);
    status = __nvoc_ctor_RmResource(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RmResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RmResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RmResource));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RmResource(RmResource **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RmResource(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

