#define NVOC_VASPACE_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_vaspace_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__cd048b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VaSpaceApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for VaSpaceApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__VaSpaceApi(VaSpaceApi*);
void __nvoc_init_funcTable_VaSpaceApi(VaSpaceApi*);
NV_STATUS __nvoc_ctor_VaSpaceApi(VaSpaceApi*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_VaSpaceApi(VaSpaceApi*);
void __nvoc_dtor_VaSpaceApi(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__VaSpaceApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__VaSpaceApi;

// Down-thunk(s) to bridge VaSpaceApi methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_VaSpaceApi_resCanCopy(struct RsResource *pResource);    // this

// Up-thunk(s) to bridge VaSpaceApi methods to ancestors (if any)

// Class-specific details for VaSpaceApi
const struct NVOC_CLASS_DEF __nvoc_class_def_VaSpaceApi = 
{
    .classInfo.size =               sizeof(VaSpaceApi),
    .classInfo.classId =            classId(VaSpaceApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "VaSpaceApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_VaSpaceApi,
    .pCastInfo =          &__nvoc_castinfo__VaSpaceApi,
    .pExportInfo =        &__nvoc_export_info__VaSpaceApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat__EXPORT(void *pVaspaceApi, void *pGmmuFormatParams) {
    return vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL(pVaspaceApi, pGmmuFormatParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00018000u)
static NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__EXPORT(void *pVaspaceApi, void *pPageLevelInfoParams) {
    return vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(pVaspaceApi, pPageLevelInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00018000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00018000u)
static NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries__EXPORT(void *pVaspaceApi, void *pReserveEntriesParams) {
    return vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL(pVaspaceApi, pReserveEntriesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00018000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00018000u)
static NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries__EXPORT(void *pVaspaceApi, void *pReleaseEntriesParams) {
    return vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL(pVaspaceApi, pReleaseEntriesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00018000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014004u)
static NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__EXPORT(void *pVaspaceApi, void *pCopyServerReservedPdesParams) {
    return vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL(pVaspaceApi, pCopyServerReservedPdesParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__EXPORT(void *pVaspaceApi, void *pVaspaceGetHostRmManagedSizeParams) {
    return vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VaSpaceApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vaspaceapiCtrlCmdVaspaceGetGmmuFormat__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10101u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetGmmuFormat"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18000u)
        /*flags=*/      0x18000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10102u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetPageLevelInfo"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vaspaceapiCtrlCmdVaspaceReserveEntries__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18000u)
        /*flags=*/      0x18000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10103u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceReserveEntries"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vaspaceapiCtrlCmdVaspaceReleaseEntries__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18000u)
        /*flags=*/      0x18000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10104u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceReleaseEntries"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*flags=*/      0x14004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10106u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10107u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__VaSpaceApi __nvoc_metadata__VaSpaceApi = {
    .rtti.pClassDef = &__nvoc_class_def_VaSpaceApi,    // (vaspaceapi) this
    .rtti.dtor      = &__nvoc_dtor_VaSpaceApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_VaSpaceApi_resCanCopy,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
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
const struct NVOC_CASTINFO __nvoc_castinfo__VaSpaceApi = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__VaSpaceApi.rtti,    // [0]: (vaspaceapi) this
        &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in VaSpaceApi from superclasses

// vaspaceapiCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_VaSpaceApi_resCanCopy(struct RsResource *pResource) {
    return vaspaceapiCanCopy_IMPL((struct VaSpaceApi *)(((unsigned char *) pResource) - NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__VaSpaceApi = 
{
    .numEntries=     6,
    .pExportEntries= __nvoc_exported_method_def_VaSpaceApi
};


// Destruct VaSpaceApi object.
void __nvoc_vaspaceapiDestruct(VaSpaceApi*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_VaSpaceApi(Dynamic* pThis) {

    VaSpaceApi *__nvoc_this = (VaSpaceApi *) pThis;

// Call destructor.
    __nvoc_vaspaceapiDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_VaSpaceApi(VaSpaceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct VaSpaceApi object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_VaSpaceApi(VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pResource->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VaSpaceApi_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_VaSpaceApi(pResource);

    // Call the constructor for this class.
    status = __nvoc_vaspaceapiConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VaSpaceApi_fail__init;
    goto __nvoc_ctor_VaSpaceApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_VaSpaceApi_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pResource->__nvoc_base_GpuResource);
__nvoc_ctor_VaSpaceApi_fail_GpuResource:
__nvoc_ctor_VaSpaceApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VaSpaceApi_1(VaSpaceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_VaSpaceApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_VaSpaceApi(VaSpaceApi *pThis) {
    __nvoc_init_funcTable_VaSpaceApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__VaSpaceApi(VaSpaceApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_VaSpaceApi = pThis;    // (vaspaceapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__VaSpaceApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__VaSpaceApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__VaSpaceApi;    // (vaspaceapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_VaSpaceApi(pThis);
}

NV_STATUS __nvoc_objCreate_VaSpaceApi(VaSpaceApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    VaSpaceApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(VaSpaceApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(VaSpaceApi));

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
    __nvoc_init__VaSpaceApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_VaSpaceApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_VaSpaceApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_VaSpaceApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(VaSpaceApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_VaSpaceApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_VaSpaceApi((VaSpaceApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

