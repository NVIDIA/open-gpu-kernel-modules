#define NVOC_VIRT_MEM_RANGE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_virt_mem_range_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__7032c6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemoryRange;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemory;

// Forward declarations for VirtualMemoryRange
void __nvoc_init__VirtualMemory(VirtualMemory*);
void __nvoc_init__VirtualMemoryRange(VirtualMemoryRange*);
void __nvoc_init_funcTable_VirtualMemoryRange(VirtualMemoryRange*);
NV_STATUS __nvoc_ctor_VirtualMemoryRange(VirtualMemoryRange*, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_VirtualMemoryRange(VirtualMemoryRange*);
void __nvoc_dtor_VirtualMemoryRange(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__VirtualMemoryRange;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__VirtualMemoryRange;

// Down-thunk(s) to bridge VirtualMemoryRange methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^4
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^4
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
NV_STATUS __nvoc_down_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super^3
NvBool __nvoc_down_thunk_StandardMemory_resCanCopy(struct RsResource *pStandardMemory);    // super^2
NV_STATUS __nvoc_down_thunk_VirtualMemory_resMapTo(struct RsResource *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_down_thunk_VirtualMemory_resUnmapFrom(struct RsResource *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvBool __nvoc_down_thunk_VirtualMemory_resIsPartialUnmapSupported(struct RsResource *pVirtualMemory);    // super

// Up-thunk(s) to bridge VirtualMemoryRange methods to ancestors (if any)

// Class-specific details for VirtualMemoryRange
const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemoryRange = 
{
    .classInfo.size =               sizeof(VirtualMemoryRange),
    .classInfo.classId =            classId(VirtualMemoryRange),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "VirtualMemoryRange",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_VirtualMemoryRange,
    .pCastInfo =          &__nvoc_castinfo__VirtualMemoryRange,
    .pExportInfo =        &__nvoc_export_info__VirtualMemoryRange
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__VirtualMemoryRange __nvoc_metadata__VirtualMemoryRange = {
    .rtti.pClassDef = &__nvoc_class_def_VirtualMemoryRange,    // (vmrange) this
    .rtti.dtor      = &__nvoc_dtor_VirtualMemoryRange,
    .rtti.offset    = 0,
    .metadata__VirtualMemory.rtti.pClassDef = &__nvoc_class_def_VirtualMemory,    // (virtmem) super
    .metadata__VirtualMemory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory),
    .metadata__VirtualMemory.metadata__StandardMemory.rtti.pClassDef = &__nvoc_class_def_StandardMemory,    // (stdmem) super^2
    .metadata__VirtualMemory.metadata__StandardMemory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.metadata__StandardMemory.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory),
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super^3
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory),
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^4
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^5
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^6
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^5
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &__nvoc_down_thunk_VirtualMemory_resMapTo,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &__nvoc_down_thunk_VirtualMemory_resUnmapFrom,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &__nvoc_down_thunk_VirtualMemory_resIsPartialUnmapSupported,    // inline virtual body
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_StandardMemory_resCanCopy,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.vtable.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.vtable.__memIsReady__ = &memIsReady_IMPL,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.vtable.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.vtable.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__VirtualMemoryRange = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__VirtualMemoryRange.rtti,    // [0]: (vmrange) this
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.rtti,    // [1]: (virtmem) super
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.rtti,    // [2]: (stdmem) super^2
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.rtti,    // [3]: (mem) super^3
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti,    // [4]: (rmres) super^4
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [5]: (res) super^5
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [6]: (obj) super^6
        &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [7]: (rmrescmn) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__VirtualMemoryRange = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct VirtualMemoryRange object.
void __nvoc_dtor_VirtualMemory(Dynamic*);
void __nvoc_dtor_VirtualMemoryRange(Dynamic* pThis) {

    VirtualMemoryRange *__nvoc_this = (VirtualMemoryRange *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_VirtualMemory((Dynamic *) &__nvoc_this->__nvoc_base_VirtualMemory);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_VirtualMemoryRange(VirtualMemoryRange *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct VirtualMemoryRange object.
NV_STATUS __nvoc_ctor_VirtualMemory(VirtualMemory *pVirtualMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_VirtualMemoryRange(VirtualMemoryRange *pVmRange, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_VirtualMemory(&pVmRange->__nvoc_base_VirtualMemory, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VirtualMemoryRange_fail_VirtualMemory;

    // Initialize data fields.
    __nvoc_init_dataField_VirtualMemoryRange(pVmRange);

    // Call the constructor for this class.
    status = __nvoc_vmrangeConstruct(pVmRange, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VirtualMemoryRange_fail__init;
    goto __nvoc_ctor_VirtualMemoryRange_exit; // Success

    // Unwind on error.
__nvoc_ctor_VirtualMemoryRange_fail__init:
    __nvoc_dtor_VirtualMemory((Dynamic *)&pVmRange->__nvoc_base_VirtualMemory);
__nvoc_ctor_VirtualMemoryRange_fail_VirtualMemory:
__nvoc_ctor_VirtualMemoryRange_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VirtualMemoryRange_1(VirtualMemoryRange *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_VirtualMemoryRange_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_VirtualMemoryRange(VirtualMemoryRange *pThis) {
    __nvoc_init_funcTable_VirtualMemoryRange_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__VirtualMemoryRange(VirtualMemoryRange *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^6
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^5
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^5
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^4
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory;    // (mem) super^3
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory;    // (stdmem) super^2
    pThis->__nvoc_pbase_VirtualMemory = &pThis->__nvoc_base_VirtualMemory;    // (virtmem) super
    pThis->__nvoc_pbase_VirtualMemoryRange = pThis;    // (vmrange) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__VirtualMemory(&pThis->__nvoc_base_VirtualMemory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^6
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^5
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^5
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource;    // (rmres) super^4
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory.metadata__Memory;    // (mem) super^3
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory.metadata__StandardMemory;    // (stdmem) super^2
    pThis->__nvoc_base_VirtualMemory.__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange.metadata__VirtualMemory;    // (virtmem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__VirtualMemoryRange;    // (vmrange) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_VirtualMemoryRange(pThis);
}

NV_STATUS __nvoc_objCreate_VirtualMemoryRange(VirtualMemoryRange **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    VirtualMemoryRange *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(VirtualMemoryRange));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(VirtualMemoryRange));

    __nvoc_pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__VirtualMemoryRange(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_VirtualMemoryRange(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_VirtualMemoryRange_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_VirtualMemoryRange_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(VirtualMemoryRange));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_VirtualMemoryRange(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_VirtualMemoryRange((VirtualMemoryRange **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

