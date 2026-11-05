#define NVOC_MEM_FABRIC_IMPORT_V2_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_mem_fabric_import_v2_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f96871 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportV2;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

// Forward declarations for MemoryFabricImportV2
void __nvoc_init__Memory(Memory*);
void __nvoc_init__MemoryFabricImportV2(MemoryFabricImportV2*);
void __nvoc_init_funcTable_MemoryFabricImportV2(MemoryFabricImportV2*);
NV_STATUS __nvoc_ctor_MemoryFabricImportV2(MemoryFabricImportV2*, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_MemoryFabricImportV2(MemoryFabricImportV2*);
void __nvoc_dtor_MemoryFabricImportV2(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryFabricImportV2;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryFabricImportV2;

// Down-thunk(s) to bridge MemoryFabricImportV2 methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super
NV_STATUS __nvoc_down_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super
NvBool __nvoc_down_thunk_MemoryFabricImportV2_resCanCopy(struct RsResource *pMemoryFabricImportV2);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memIsReady(struct Memory *pMemoryFabricImportV2, NvBool bCopyConstructorContext);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_resControl(struct RsResource *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed(struct Memory *pMemoryFabricImportV2, struct OBJGPU *pGpu);    // this
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed(struct Memory *pMemoryFabricImportV2);    // this

// Up-thunk(s) to bridge MemoryFabricImportV2 methods to ancestors (if any)

// Class-specific details for MemoryFabricImportV2
const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportV2 = 
{
    .classInfo.size =               sizeof(MemoryFabricImportV2),
    .classInfo.classId =            classId(MemoryFabricImportV2),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MemoryFabricImportV2",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MemoryFabricImportV2,
    .pCastInfo =          &__nvoc_castinfo__MemoryFabricImportV2,
    .pExportInfo =        &__nvoc_export_info__MemoryFabricImportV2
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS memoryfabricimportv2CtrlGetInfo__EXPORT(void *pMemoryFabricImportV2, void *pParams) {
    return memoryfabricimportv2CtrlGetInfo_IMPL(pMemoryFabricImportV2, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryFabricImportV2[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricimportv2CtrlGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf90101u,
        /*paramSize=*/  sizeof(NV00F9_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabricImportV2.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricimportv2CtrlGetInfo"
#endif
    },
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__MemoryFabricImportV2 __nvoc_metadata__MemoryFabricImportV2 = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryFabricImportV2,    // (memoryfabricimportv2) this
    .rtti.dtor      = &__nvoc_dtor_MemoryFabricImportV2,
    .rtti.offset    = 0,
    .metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super
    .metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory),
    .metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabricImportV2_resCanCopy,    // virtual
    .metadata__Memory.vtable.__memIsReady__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsReady,    // virtual
    .vtable.__memoryfabricimportv2CopyConstruct__ = &memoryfabricimportv2CopyConstruct_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_MemoryFabricImportV2_resControl,    // virtual
    .metadata__Memory.vtable.__memGetMapAddrSpace__ = &__nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace,    // virtual
    .metadata__Memory.vtable.__memIsGpuMapAllowed__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed,    // inline virtual body
    .metadata__Memory.vtable.__memIsExportAllowed__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed,    // inline virtual body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryFabricImportV2 = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MemoryFabricImportV2.rtti,    // [0]: (memoryfabricimportv2) this
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.rtti,    // [1]: (mem) super
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 6 down-thunk(s) defined to bridge methods in MemoryFabricImportV2 from superclasses

// memoryfabricimportv2CanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_MemoryFabricImportV2_resCanCopy(struct RsResource *pMemoryFabricImportV2) {
    return memoryfabricimportv2CanCopy_IMPL((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportv2IsReady: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memIsReady(struct Memory *pMemoryFabricImportV2, NvBool bCopyConstructorContext) {
    return memoryfabricimportv2IsReady_IMPL((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), bCopyConstructorContext);
}

// memoryfabricimportv2Control: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_resControl(struct RsResource *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryfabricimportv2Control_IMPL((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memoryfabricimportv2GetMapAddrSpace: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memoryfabricimportv2GetMapAddrSpace_IMPL((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// memoryfabricimportv2IsGpuMapAllowed: virtual override (mem) base (mem)
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed(struct Memory *pMemoryFabricImportV2, struct OBJGPU *pGpu) {
    return memoryfabricimportv2IsGpuMapAllowed_IMPL((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pGpu);
}

// memoryfabricimportv2IsExportAllowed: virtual override (mem) base (mem)
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed(struct Memory *pMemoryFabricImportV2) {
    return memoryfabricimportv2IsExportAllowed_IMPL((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryFabricImportV2 = 
{
    .numEntries=     1,
    .pExportEntries= __nvoc_exported_method_def_MemoryFabricImportV2
};


// Destruct MemoryFabricImportV2 object.
void __nvoc_memoryfabricimportv2Destruct(MemoryFabricImportV2*);
void __nvoc_dtor_Memory(Dynamic*);
void __nvoc_dtor_MemoryFabricImportV2(Dynamic* pThis) {

    MemoryFabricImportV2 *__nvoc_this = (MemoryFabricImportV2 *) pThis;

// Call destructor.
    __nvoc_memoryfabricimportv2Destruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_Memory((Dynamic *) &__nvoc_this->__nvoc_base_Memory);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MemoryFabricImportV2 object.
NV_STATUS __nvoc_ctor_Memory(Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_MemoryFabricImportV2(MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Memory(&pMemoryFabricImportV2->__nvoc_base_Memory, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabricImportV2_fail_Memory;

    // Initialize data fields.
    __nvoc_init_dataField_MemoryFabricImportV2(pMemoryFabricImportV2);

    // Call the constructor for this class.
    status = __nvoc_memoryfabricimportv2Construct(pMemoryFabricImportV2, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabricImportV2_fail__init;
    goto __nvoc_ctor_MemoryFabricImportV2_exit; // Success

    // Unwind on error.
__nvoc_ctor_MemoryFabricImportV2_fail__init:
    __nvoc_dtor_Memory((Dynamic *)&pMemoryFabricImportV2->__nvoc_base_Memory);
__nvoc_ctor_MemoryFabricImportV2_fail_Memory:
__nvoc_ctor_MemoryFabricImportV2_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryFabricImportV2_1(MemoryFabricImportV2 *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MemoryFabricImportV2_1


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {
    __nvoc_init_funcTable_MemoryFabricImportV2_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;    // (mem) super
    pThis->__nvoc_pbase_MemoryFabricImportV2 = pThis;    // (memoryfabricimportv2) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Memory(&pThis->__nvoc_base_Memory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory;    // (mem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2;    // (memoryfabricimportv2) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryFabricImportV2(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryFabricImportV2(MemoryFabricImportV2 **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MemoryFabricImportV2 *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MemoryFabricImportV2));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MemoryFabricImportV2));

    __nvoc_pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__MemoryFabricImportV2(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MemoryFabricImportV2(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MemoryFabricImportV2_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MemoryFabricImportV2_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MemoryFabricImportV2));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportV2(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_MemoryFabricImportV2((MemoryFabricImportV2 **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

