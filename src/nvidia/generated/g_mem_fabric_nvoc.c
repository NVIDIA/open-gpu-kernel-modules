#define NVOC_MEM_FABRIC_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_mem_fabric_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__127499 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

// Forward declarations for MemoryFabric
void __nvoc_init__Memory(Memory*);
void __nvoc_init__MemoryFabric(MemoryFabric*);
void __nvoc_init_funcTable_MemoryFabric(MemoryFabric*);
NV_STATUS __nvoc_ctor_MemoryFabric(MemoryFabric*, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_MemoryFabric(MemoryFabric*);
void __nvoc_dtor_MemoryFabric(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryFabric;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryFabric;

// Down-thunk(s) to bridge MemoryFabric methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_MemoryFabric_resUnmapFrom(struct RsResource *pMemoryFabric, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvBool __nvoc_down_thunk_MemoryFabric_resCanCopy(struct RsResource *pMemoryFabric);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabric_resControl(struct RsResource *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge MemoryFabric methods to ancestors (if any)

// Class-specific details for MemoryFabric
const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric = 
{
    .classInfo.size =               sizeof(MemoryFabric),
    .classInfo.classId =            classId(MemoryFabric),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MemoryFabric",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MemoryFabric,
    .pCastInfo =          &__nvoc_castinfo__MemoryFabric,
    .pExportInfo =        &__nvoc_export_info__MemoryFabric
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS memoryfabricCtrlGetInfo__EXPORT(void *pMemoryFabric, void *pParams) {
    return memoryfabricCtrlGetInfo_IMPL(pMemoryFabric, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010318u)
static NV_STATUS memoryfabricCtrlCmdDescribe__EXPORT(void *pMemoryFabric, void *pParams) {
    return memoryfabricCtrlCmdDescribe_IMPL(pMemoryFabric, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010318u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000918u)
static NV_STATUS memoryfabricCtrlAttachMem__EXPORT(void *pMemoryFabric, void *pParams) {
    return memoryfabricCtrlAttachMem_IMPL(pMemoryFabric, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000918u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000918u)
static NV_STATUS memoryfabricCtrlDetachMem__EXPORT(void *pMemoryFabric, void *pParams) {
    return memoryfabricCtrlDetachMem_IMPL(pMemoryFabric, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000918u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)
static NV_STATUS memoryfabricCtrlGetNumAttachedMem__EXPORT(void *pMemoryFabric, void *pParams) {
    return memoryfabricCtrlGetNumAttachedMem_IMPL(pMemoryFabric, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000118u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000110u)
static NV_STATUS memoryfabricCtrlGetPageLevelInfo__EXPORT(void *pMemoryFabric, void *pParams) {
    return memoryfabricCtrlGetPageLevelInfo_IMPL(pMemoryFabric, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000110u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryFabric[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricCtrlGetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80101u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10318u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricCtrlCmdDescribe__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10318u)
        /*flags=*/      0x10318u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80102u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_DESCRIBE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlCmdDescribe"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricCtrlAttachMem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*flags=*/      0x918u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80103u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_ATTACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlAttachMem"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricCtrlDetachMem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*flags=*/      0x918u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80104u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_DETACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlDetachMem"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricCtrlGetNumAttachedMem__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80105u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetNumAttachedMem"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &memoryfabricCtrlGetPageLevelInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*flags=*/      0x110u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80107u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetPageLevelInfo"
#endif
    },
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__MemoryFabric __nvoc_metadata__MemoryFabric = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryFabric,    // (memoryfabric) this
    .rtti.dtor      = &__nvoc_dtor_MemoryFabric,
    .rtti.offset    = 0,
    .metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super
    .metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.rtti.offset    = NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory),
    .metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &__nvoc_down_thunk_MemoryFabric_resUnmapFrom,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabric_resCanCopy,    // virtual
    .vtable.__memoryfabricCopyConstruct__ = &memoryfabricCopyConstruct_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_MemoryFabric_resControl,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .metadata__Memory.vtable.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .metadata__Memory.vtable.__memIsReady__ = &memIsReady_IMPL,    // virtual
    .metadata__Memory.vtable.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
    .metadata__Memory.vtable.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
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
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryFabric = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MemoryFabric.rtti,    // [0]: (memoryfabric) this
        &__nvoc_metadata__MemoryFabric.metadata__Memory.rtti,    // [1]: (mem) super
        &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 3 down-thunk(s) defined to bridge methods in MemoryFabric from superclasses

// memoryfabricUnmapFrom: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabric_resUnmapFrom(struct RsResource *pMemoryFabric, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return memoryfabricUnmapFrom_IMPL((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricCanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_MemoryFabric_resCanCopy(struct RsResource *pMemoryFabric) {
    return memoryfabricCanCopy_IMPL((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricControl: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabric_resControl(struct RsResource *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryfabricControl_IMPL((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryFabric = 
{
    .numEntries=     6,
    .pExportEntries= __nvoc_exported_method_def_MemoryFabric
};


// Destruct MemoryFabric object.
void __nvoc_memoryfabricDestruct(MemoryFabric*);
void __nvoc_dtor_Memory(Dynamic*);
void __nvoc_dtor_MemoryFabric(Dynamic* pThis) {

    MemoryFabric *__nvoc_this = (MemoryFabric *) pThis;

// Call destructor.
    __nvoc_memoryfabricDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_Memory((Dynamic *) &__nvoc_this->__nvoc_base_Memory);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MemoryFabric(MemoryFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MemoryFabric object.
NV_STATUS __nvoc_ctor_Memory(Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_MemoryFabric(MemoryFabric *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Memory(&pMemoryFabric->__nvoc_base_Memory, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabric_fail_Memory;

    // Initialize data fields.
    __nvoc_init_dataField_MemoryFabric(pMemoryFabric);

    // Call the constructor for this class.
    status = __nvoc_memoryfabricConstruct(pMemoryFabric, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabric_fail__init;
    goto __nvoc_ctor_MemoryFabric_exit; // Success

    // Unwind on error.
__nvoc_ctor_MemoryFabric_fail__init:
    __nvoc_dtor_Memory((Dynamic *)&pMemoryFabric->__nvoc_base_Memory);
__nvoc_ctor_MemoryFabric_fail_Memory:
__nvoc_ctor_MemoryFabric_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryFabric_1(MemoryFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MemoryFabric_1


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_MemoryFabric(MemoryFabric *pThis) {
    __nvoc_init_funcTable_MemoryFabric_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryFabric(MemoryFabric *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;    // (mem) super
    pThis->__nvoc_pbase_MemoryFabric = pThis;    // (memoryfabric) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Memory(&pThis->__nvoc_base_Memory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabric.metadata__Memory.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabric.metadata__Memory;    // (mem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabric;    // (memoryfabric) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryFabric(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryFabric(MemoryFabric **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MemoryFabric *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MemoryFabric));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MemoryFabric));

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
    __nvoc_init__MemoryFabric(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MemoryFabric(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MemoryFabric_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MemoryFabric_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MemoryFabric));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryFabric(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_MemoryFabric((MemoryFabric **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

