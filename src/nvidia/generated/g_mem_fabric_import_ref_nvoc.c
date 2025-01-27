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

// Down-thunk(s) to bridge MemoryFabricImportedRef methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_down_thunk_MemoryFabricImportedRef_resCanCopy(struct RsResource *pMemoryFabricImportedRef);    // this

// 1 down-thunk(s) defined to bridge methods in MemoryFabricImportedRef from superclasses

// memoryfabricimportedrefCanCopy: virtual override (res) base (rmres)
NvBool __nvoc_down_thunk_MemoryFabricImportedRef_resCanCopy(struct RsResource *pMemoryFabricImportedRef) {
    return memoryfabricimportedrefCanCopy((struct MemoryFabricImportedRef *)(((unsigned char *) pMemoryFabricImportedRef) - NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// Up-thunk(s) to bridge MemoryFabricImportedRef methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportedrefAccessCallback(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportedrefShareCallback(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemInterMapParams(struct MemoryFabricImportedRef *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefCheckMemInterUnmap(struct MemoryFabricImportedRef *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemoryMappingDescriptor(struct MemoryFabricImportedRef *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Prologue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Epilogue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Prologue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Epilogue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefIsDuplicate(struct MemoryFabricImportedRef *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_memoryfabricimportedrefPreDestruct(struct MemoryFabricImportedRef *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefControl(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefControlFilter(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefMap(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmap(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_memoryfabricimportedrefIsPartialUnmapSupported(struct MemoryFabricImportedRef *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefMapTo(struct MemoryFabricImportedRef *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmapFrom(struct MemoryFabricImportedRef *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_memoryfabricimportedrefGetRefCount(struct MemoryFabricImportedRef *pResource);    // this
void __nvoc_up_thunk_RsResource_memoryfabricimportedrefAddAdditionalDependants(struct RsClient *pClient, struct MemoryFabricImportedRef *pResource, RsResourceRef *pReference);    // this

// 20 up-thunk(s) defined to bridge methods in MemoryFabricImportedRef to superclasses

// memoryfabricimportedrefAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportedrefAccessCallback(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// memoryfabricimportedrefShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportedrefShareCallback(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryfabricimportedrefGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemInterMapParams(struct MemoryFabricImportedRef *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pParams);
}

// memoryfabricimportedrefCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefCheckMemInterUnmap(struct MemoryFabricImportedRef *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// memoryfabricimportedrefGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemoryMappingDescriptor(struct MemoryFabricImportedRef *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), ppMemDesc);
}

// memoryfabricimportedrefControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Prologue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportedrefControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Epilogue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportedrefControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Prologue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportedrefControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Epilogue(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportedrefIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefIsDuplicate(struct MemoryFabricImportedRef *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// memoryfabricimportedrefPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_memoryfabricimportedrefPreDestruct(struct MemoryFabricImportedRef *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportedrefControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefControl(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memoryfabricimportedrefControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefControlFilter(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memoryfabricimportedrefMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefMap(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// memoryfabricimportedrefUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmap(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// memoryfabricimportedrefIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_memoryfabricimportedrefIsPartialUnmapSupported(struct MemoryFabricImportedRef *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportedrefMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefMapTo(struct MemoryFabricImportedRef *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricimportedrefUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmapFrom(struct MemoryFabricImportedRef *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricimportedrefGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_memoryfabricimportedrefGetRefCount(struct MemoryFabricImportedRef *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportedrefAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_memoryfabricimportedrefAddAdditionalDependants(struct RsClient *pClient, struct MemoryFabricImportedRef *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportedRef, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
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

    // memoryfabricimportedrefCtrlValidate -- exported (id=0xfb0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memoryfabricimportedrefCtrlValidate__ = &memoryfabricimportedrefCtrlValidate_IMPL;
#endif
} // End __nvoc_init_funcTable_MemoryFabricImportedRef_1 with approximately 1 basic block(s).


// Initialize vtable(s) for 22 virtual method(s).
void __nvoc_init_funcTable_MemoryFabricImportedRef(MemoryFabricImportedRef *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__MemoryFabricImportedRef vtable = {
        .__memoryfabricimportedrefCanCopy__ = &memoryfabricimportedrefCanCopy_IMPL,    // virtual override (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabricImportedRef_resCanCopy,    // virtual
        .__memoryfabricimportedrefAccessCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__memoryfabricimportedrefShareCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__memoryfabricimportedrefGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__memoryfabricimportedrefCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__memoryfabricimportedrefGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__memoryfabricimportedrefControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__memoryfabricimportedrefControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__memoryfabricimportedrefControl_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__memoryfabricimportedrefControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportedrefControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__memoryfabricimportedrefIsDuplicate__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__memoryfabricimportedrefPreDestruct__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__memoryfabricimportedrefControl__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__memoryfabricimportedrefControlFilter__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__memoryfabricimportedrefMap__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__memoryfabricimportedrefUnmap__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__memoryfabricimportedrefIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__memoryfabricimportedrefMapTo__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefMapTo,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__memoryfabricimportedrefUnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefUnmapFrom,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__memoryfabricimportedrefGetRefCount__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__memoryfabricimportedrefAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryfabricimportedrefAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_vtable = &vtable;    // (memoryfabricimportedref) this

    // Initialize vtable(s) with 1 per-object function pointer(s).
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

