#define NVOC_LOCK_TEST_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_lock_test_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__19e861 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LockTestRelaxedDupObject;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for LockTestRelaxedDupObject
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__LockTestRelaxedDupObject(LockTestRelaxedDupObject*);
void __nvoc_init_funcTable_LockTestRelaxedDupObject(LockTestRelaxedDupObject*);
NV_STATUS __nvoc_ctor_LockTestRelaxedDupObject(LockTestRelaxedDupObject*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_LockTestRelaxedDupObject(LockTestRelaxedDupObject*);
void __nvoc_dtor_LockTestRelaxedDupObject(LockTestRelaxedDupObject*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LockTestRelaxedDupObject;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LockTestRelaxedDupObject;

// Down-thunk(s) to bridge LockTestRelaxedDupObject methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_down_thunk_LockTestRelaxedDupObject_resCanCopy(struct RsResource *pResource);    // this

// Up-thunk(s) to bridge LockTestRelaxedDupObject methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjAccessCallback(struct LockTestRelaxedDupObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjShareCallback(struct LockTestRelaxedDupObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjGetMemInterMapParams(struct LockTestRelaxedDupObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjCheckMemInterUnmap(struct LockTestRelaxedDupObject *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjGetMemoryMappingDescriptor(struct LockTestRelaxedDupObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControlSerialization_Prologue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControlSerialization_Epilogue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControl_Prologue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControl_Epilogue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjIsDuplicate(struct LockTestRelaxedDupObject *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjPreDestruct(struct LockTestRelaxedDupObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjControl(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjControlFilter(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjMap(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjUnmap(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjIsPartialUnmapSupported(struct LockTestRelaxedDupObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjMapTo(struct LockTestRelaxedDupObject *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjUnmapFrom(struct LockTestRelaxedDupObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjGetRefCount(struct LockTestRelaxedDupObject *pResource);    // this
void __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjAddAdditionalDependants(struct RsClient *pClient, struct LockTestRelaxedDupObject *pResource, RsResourceRef *pReference);    // this

// Class-specific details for LockTestRelaxedDupObject
const struct NVOC_CLASS_DEF __nvoc_class_def_LockTestRelaxedDupObject = 
{
    .classInfo.size =               sizeof(LockTestRelaxedDupObject),
    .classInfo.classId =            classId(LockTestRelaxedDupObject),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LockTestRelaxedDupObject",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_LockTestRelaxedDupObject,
    .pCastInfo =          &__nvoc_castinfo__LockTestRelaxedDupObject,
    .pExportInfo =        &__nvoc_export_info__LockTestRelaxedDupObject
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__LockTestRelaxedDupObject __nvoc_metadata__LockTestRelaxedDupObject = {
    .rtti.pClassDef = &__nvoc_class_def_LockTestRelaxedDupObject,    // (lockTestRelaxedDupObj) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_LockTestRelaxedDupObject,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__lockTestRelaxedDupObjCanCopy__ = &lockTestRelaxedDupObjCanCopy_e661f0,    // inline virtual override (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_LockTestRelaxedDupObject_resCanCopy,    // virtual
    .vtable.__lockTestRelaxedDupObjAccessCallback__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__lockTestRelaxedDupObjShareCallback__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__lockTestRelaxedDupObjGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__lockTestRelaxedDupObjControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__lockTestRelaxedDupObjControl_Prologue__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__lockTestRelaxedDupObjControl_Epilogue__ = &__nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__lockTestRelaxedDupObjIsDuplicate__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjPreDestruct__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjControl__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjControlFilter__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjMap__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjUnmap__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__lockTestRelaxedDupObjMapTo__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjUnmapFrom__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjGetRefCount__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__lockTestRelaxedDupObjAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_lockTestRelaxedDupObjAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LockTestRelaxedDupObject = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__LockTestRelaxedDupObject.rtti,    // [0]: (lockTestRelaxedDupObj) this
        &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in LockTestRelaxedDupObject from superclasses

// lockTestRelaxedDupObjCanCopy: inline virtual override (res) base (rmres) body
NvBool __nvoc_down_thunk_LockTestRelaxedDupObject_resCanCopy(struct RsResource *pResource) {
    return lockTestRelaxedDupObjCanCopy((struct LockTestRelaxedDupObject *)(((unsigned char *) pResource) - NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 20 up-thunk(s) defined to bridge methods in LockTestRelaxedDupObject to superclasses

// lockTestRelaxedDupObjAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjAccessCallback(struct LockTestRelaxedDupObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// lockTestRelaxedDupObjShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjShareCallback(struct LockTestRelaxedDupObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// lockTestRelaxedDupObjGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjGetMemInterMapParams(struct LockTestRelaxedDupObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pParams);
}

// lockTestRelaxedDupObjCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjCheckMemInterUnmap(struct LockTestRelaxedDupObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// lockTestRelaxedDupObjGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjGetMemoryMappingDescriptor(struct LockTestRelaxedDupObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), ppMemDesc);
}

// lockTestRelaxedDupObjControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControlSerialization_Prologue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pCallContext, pParams);
}

// lockTestRelaxedDupObjControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControlSerialization_Epilogue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pCallContext, pParams);
}

// lockTestRelaxedDupObjControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControl_Prologue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pCallContext, pParams);
}

// lockTestRelaxedDupObjControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_lockTestRelaxedDupObjControl_Epilogue(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource)), pCallContext, pParams);
}

// lockTestRelaxedDupObjIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjIsDuplicate(struct LockTestRelaxedDupObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// lockTestRelaxedDupObjPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjPreDestruct(struct LockTestRelaxedDupObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// lockTestRelaxedDupObjControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjControl(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// lockTestRelaxedDupObjControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjControlFilter(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// lockTestRelaxedDupObjMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjMap(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// lockTestRelaxedDupObjUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjUnmap(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// lockTestRelaxedDupObjIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjIsPartialUnmapSupported(struct LockTestRelaxedDupObject *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// lockTestRelaxedDupObjMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjMapTo(struct LockTestRelaxedDupObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// lockTestRelaxedDupObjUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjUnmapFrom(struct LockTestRelaxedDupObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// lockTestRelaxedDupObjGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjGetRefCount(struct LockTestRelaxedDupObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// lockTestRelaxedDupObjAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_lockTestRelaxedDupObjAddAdditionalDependants(struct RsClient *pClient, struct LockTestRelaxedDupObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(LockTestRelaxedDupObject, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__LockTestRelaxedDupObject = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LockTestRelaxedDupObject object.
void __nvoc_lockTestRelaxedDupObjDestruct(LockTestRelaxedDupObject*);
void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_LockTestRelaxedDupObject(LockTestRelaxedDupObject* pThis) {

// Call destructor.
    __nvoc_lockTestRelaxedDupObjDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_LockTestRelaxedDupObject(LockTestRelaxedDupObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LockTestRelaxedDupObject object.
NV_STATUS __nvoc_ctor_RmResource(RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_LockTestRelaxedDupObject(LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RmResource(&pResource->__nvoc_base_RmResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_LockTestRelaxedDupObject_fail_RmResource;

    // Initialize data fields.
    __nvoc_init_dataField_LockTestRelaxedDupObject(pResource);

    // Call the constructor for this class.
    status = __nvoc_lockTestRelaxedDupObjConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_LockTestRelaxedDupObject_fail__init;
    goto __nvoc_ctor_LockTestRelaxedDupObject_exit; // Success

    // Unwind on error.
__nvoc_ctor_LockTestRelaxedDupObject_fail__init:
    __nvoc_dtor_RmResource(&pResource->__nvoc_base_RmResource);
__nvoc_ctor_LockTestRelaxedDupObject_fail_RmResource:
__nvoc_ctor_LockTestRelaxedDupObject_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LockTestRelaxedDupObject_1(LockTestRelaxedDupObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LockTestRelaxedDupObject_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_LockTestRelaxedDupObject(LockTestRelaxedDupObject *pThis) {
    __nvoc_init_funcTable_LockTestRelaxedDupObject_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LockTestRelaxedDupObject(LockTestRelaxedDupObject *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_LockTestRelaxedDupObject = pThis;    // (lockTestRelaxedDupObj) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__LockTestRelaxedDupObject.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LockTestRelaxedDupObject;    // (lockTestRelaxedDupObj) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LockTestRelaxedDupObject(pThis);
}

NV_STATUS __nvoc_objCreate_LockTestRelaxedDupObject(LockTestRelaxedDupObject **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LockTestRelaxedDupObject *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LockTestRelaxedDupObject));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LockTestRelaxedDupObject));

    __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LockTestRelaxedDupObject(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LockTestRelaxedDupObject(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LockTestRelaxedDupObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LockTestRelaxedDupObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LockTestRelaxedDupObject));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LockTestRelaxedDupObject(LockTestRelaxedDupObject **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    __nvoc_status = __nvoc_objCreate_LockTestRelaxedDupObject(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);

    return __nvoc_status;
}

