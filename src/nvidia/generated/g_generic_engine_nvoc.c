#define NVOC_GENERIC_ENGINE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_generic_engine_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4bc329 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for GenericEngineApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__GenericEngineApi(GenericEngineApi*);
void __nvoc_init_funcTable_GenericEngineApi(GenericEngineApi*);
NV_STATUS __nvoc_ctor_GenericEngineApi(GenericEngineApi*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_GenericEngineApi(GenericEngineApi*);
void __nvoc_dtor_GenericEngineApi(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GenericEngineApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GenericEngineApi;

// Down-thunk(s) to bridge GenericEngineApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_GenericEngineApi_resMap(struct RsResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_down_thunk_GenericEngineApi_gpuresGetMapAddrSpace(struct GpuResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_down_thunk_GenericEngineApi_resControl(struct RsResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge GenericEngineApi methods to ancestors (if any)

// Class-specific details for GenericEngineApi
const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi = 
{
    .classInfo.size =               sizeof(GenericEngineApi),
    .classInfo.classId =            classId(GenericEngineApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GenericEngineApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GenericEngineApi,
    .pCastInfo =          &__nvoc_castinfo__GenericEngineApi,
    .pExportInfo =        &__nvoc_export_info__GenericEngineApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask__EXPORT(void *pGenericEngineApi, void *pParams) {
    return genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL(pGenericEngineApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__EXPORT(void *pGenericEngineApi, void *pParams) {
    return genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL(pGenericEngineApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)
static NV_STATUS genapiCtrlCmdBBXGetLastFlushTime__EXPORT(void *pGenericEngineApi, void *pParams) {
    return genapiCtrlCmdBBXGetLastFlushTime_IMPL(pGenericEngineApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)
static NV_STATUS genapiCtrlCmdBBXIsNVMFlushEnabled__EXPORT(void *pGenericEngineApi, void *pParams) {
    return genapiCtrlCmdBBXIsNVMFlushEnabled_IMPL(pGenericEngineApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)
static NV_STATUS genapiCtrlCmdBBXGetTimeData__EXPORT(void *pGenericEngineApi, void *pParams) {
    return genapiCtrlCmdBBXGetTimeData_IMPL(pGenericEngineApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000058u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GenericEngineApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &genapiCtrlCmdMasterGetErrorIntrOffsetMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e60101u,
        /*paramSize=*/  sizeof(NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdMasterGetErrorIntrOffsetMask"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e60102u,
        /*paramSize=*/  sizeof(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &genapiCtrlCmdBBXGetTimeData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e70102u,
        /*paramSize=*/  sizeof(NV90E7_CTRL_BBX_GET_TIME_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdBBXGetTimeData"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &genapiCtrlCmdBBXGetLastFlushTime__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e70113u,
        /*paramSize=*/  sizeof(NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdBBXGetLastFlushTime"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &genapiCtrlCmdBBXIsNVMFlushEnabled__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e70119u,
        /*paramSize=*/  sizeof(NV90E7_CTRL_BBX_IS_NVM_FLUSH_ENABLED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdBBXIsNVMFlushEnabled"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GenericEngineApi __nvoc_metadata__GenericEngineApi = {
    .rtti.pClassDef = &__nvoc_class_def_GenericEngineApi,    // (genapi) this
    .rtti.dtor      = &__nvoc_dtor_GenericEngineApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GenericEngineApi_resMap,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &__nvoc_down_thunk_GenericEngineApi_gpuresGetMapAddrSpace,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GenericEngineApi_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GenericEngineApi = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GenericEngineApi.rtti,    // [0]: (genapi) this
        &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 3 down-thunk(s) defined to bridge methods in GenericEngineApi from superclasses

// genapiMap: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_GenericEngineApi_resMap(struct RsResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return genapiMap_IMPL((struct GenericEngineApi *)(((unsigned char *) pGenericEngineApi) - NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// genapiGetMapAddrSpace: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_GenericEngineApi_gpuresGetMapAddrSpace(struct GpuResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return genapiGetMapAddrSpace_IMPL((struct GenericEngineApi *)(((unsigned char *) pGenericEngineApi) - NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// genapiControl: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_GenericEngineApi_resControl(struct RsResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return genapiControl_IMPL((struct GenericEngineApi *)(((unsigned char *) pGenericEngineApi) - NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__GenericEngineApi = 
{
    .numEntries=     5,
    .pExportEntries= __nvoc_exported_method_def_GenericEngineApi
};


// Destruct GenericEngineApi object.
void __nvoc_genapiDestruct(GenericEngineApi*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_GenericEngineApi(Dynamic* pThis) {

    GenericEngineApi *__nvoc_this = (GenericEngineApi *) pThis;

// Call destructor.
    __nvoc_genapiDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GenericEngineApi(GenericEngineApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GenericEngineApi object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_GenericEngineApi(GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pGenericEngineApi->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_GenericEngineApi_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_GenericEngineApi(pGenericEngineApi);

    // Call the constructor for this class.
    status = __nvoc_genapiConstruct(pGenericEngineApi, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_GenericEngineApi_fail__init;
    goto __nvoc_ctor_GenericEngineApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_GenericEngineApi_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pGenericEngineApi->__nvoc_base_GpuResource);
__nvoc_ctor_GenericEngineApi_fail_GpuResource:
__nvoc_ctor_GenericEngineApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GenericEngineApi_1(GenericEngineApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GenericEngineApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GenericEngineApi(GenericEngineApi *pThis) {
    __nvoc_init_funcTable_GenericEngineApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GenericEngineApi(GenericEngineApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_GenericEngineApi = pThis;    // (genapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__GenericEngineApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__GenericEngineApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GenericEngineApi;    // (genapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GenericEngineApi(pThis);
}

NV_STATUS __nvoc_objCreate_GenericEngineApi(GenericEngineApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GenericEngineApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GenericEngineApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GenericEngineApi));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GenericEngineApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GenericEngineApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GenericEngineApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GenericEngineApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GenericEngineApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GenericEngineApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_GenericEngineApi((GenericEngineApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

