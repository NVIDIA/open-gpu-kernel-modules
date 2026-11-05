#define NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_profiler_v2_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4976fc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for ProfilerBase
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__ProfilerBase(ProfilerBase*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_ProfilerBase(ProfilerBase*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase*, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_ProfilerBase(ProfilerBase*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_ProfilerBase(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerBase;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerBase;

// Down-thunk(s) to bridge ProfilerBase methods from ancestors (if any)
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

// Up-thunk(s) to bridge ProfilerBase methods to ancestors (if any)

// Class-specific details for ProfilerBase
const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase = 
{
    .classInfo.size =               sizeof(ProfilerBase),
    .classInfo.classId =            classId(ProfilerBase),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ProfilerBase",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ProfilerBase,
    .pCastInfo =          &__nvoc_castinfo__ProfilerBase,
    .pExportInfo =        &__nvoc_export_info__ProfilerBase
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010318u)
static NV_STATUS profilerBaseCtrlCmdReserveHwpmLegacy__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdReserveHwpmLegacy_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010318u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)
static NV_STATUS profilerBaseCtrlCmdInternalReserveHwpmLegacy__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS profilerBaseCtrlCmdReleaseHwpmLegacy__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReservePmAreaSmpc__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdReservePmAreaSmpc_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS profilerBaseCtrlCmdReleasePmAreaSmpc__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS profilerBaseCtrlCmdAllocPmaStream__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdAllocPmaStream_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS profilerBaseCtrlCmdFreePmaStream__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdFreePmaStream_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000500c8u)
static NV_STATUS profilerBaseCtrlCmdInternalFreePmaStream__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000500c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000500c8u)
static NV_STATUS profilerBaseCtrlCmdInternalGetMaxPmas__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalGetMaxPmas_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000500c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS profilerBaseCtrlCmdBindPmResources__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdBindPmResources_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS profilerBaseCtrlCmdUnbindPmResources__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdUnbindPmResources_DISPATCH(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)
static NV_STATUS profilerBaseCtrlCmdInternalBindPmResources__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdInternalBindPmResources_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000100c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)
static NV_STATUS profilerBaseCtrlCmdInternalUnbindPmResources__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdInternalUnbindPmResources_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000000c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS profilerBaseCtrlCmdPmaStreamUpdateGetPut__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdExecRegops__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdExecRegops_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000500c8u)
static NV_STATUS profilerBaseCtrlCmdInternalAllocPmaStream__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x000500c8u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS profilerBaseCtrlCmdInternalQuiescePmaChannel__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalQuiescePmaChannel_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050008u)
static NV_STATUS profilerBaseCtrlCmdInternalSriovPromotePmaStream__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalSriovPromotePmaStream_DISPATCH(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS profilerBaseCtrlCmdInternalPermissionsInit__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdInternalPermissionsInit_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReservePmAreaPcSampler__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReleasePmAreaPcSampler__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdGetTotalHsCredits__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdGetTotalHsCredits_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdGetChipletHsCredits__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdGetChipletHsCredits_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdGetHsCreditsMapping__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdGetHsCreditsMapping_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdGetHsCredits__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdGetHsCredits_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdSetHsCredits__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdSetHsCredits_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReserveHes__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdReserveHes_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReleaseHes__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdReleaseHes_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReserveCcuProf__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdReserveCcuProf_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS profilerBaseCtrlCmdReleaseCcuProf__EXPORT(void *pProfiler) {
    return profilerBaseCtrlCmdReleaseCcuProf_IMPL(pProfiler);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS profilerBaseCtrlCmdRequestCgControls__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdRequestCgControls_DISPATCH(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)
static NV_STATUS profilerBaseCtrlCmdReleaseCgControls__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdReleaseCgControls_DISPATCH(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00040048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS profilerBaseCtrlCmdDisableDynamicMMABoost__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS profilerBaseCtrlCmdGetDynamicMMABoostStatus__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS profilerBaseCtrlCmdPermissionQuery__EXPORT(void *pProfiler, void *pParams) {
    return profilerBaseCtrlCmdPermissionQuery_IMPL(pProfiler, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ProfilerBase[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10318u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReserveHwpmLegacy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10318u)
        /*flags=*/      0x10318u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0101u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveHwpmLegacy"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReleaseHwpmLegacy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0102u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseHwpmLegacy"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReservePmAreaSmpc__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0103u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReservePmAreaSmpc"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReleasePmAreaSmpc__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0104u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaSmpc"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdAllocPmaStream__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0105u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdAllocPmaStream"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdFreePmaStream__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0106u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdFreePmaStream"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdBindPmResources__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0107u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdBindPmResources"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdUnbindPmResources__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0108u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdUnbindPmResources"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdPmaStreamUpdateGetPut__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0109u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdPmaStreamUpdateGetPut"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdExecRegops__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdExecRegops"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReservePmAreaPcSampler__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010bu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReservePmAreaPcSampler"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReleasePmAreaPcSampler__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010cu,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaPcSampler"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdGetTotalHsCredits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010du,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetTotalHsCredits"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdSetHsCredits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010eu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_SET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdSetHsCredits"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdGetHsCredits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010fu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetHsCredits"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReserveHes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0113u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveHes"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReleaseHes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0114u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RELEASE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseHes"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdGetChipletHsCredits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0115u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetChipletHsCredits"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdGetHsCreditsMapping__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0116u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetHsCreditsMapping"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdDisableDynamicMMABoost__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0117u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdDisableDynamicMMABoost"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdGetDynamicMMABoostStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0118u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetDynamicMMABoostStatus"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReserveCcuProf__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0119u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_CCUPROF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveCcuProf"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReleaseCcuProf__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc011au,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseCcuProf"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdPermissionQuery__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc011du,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_PERMISSION_QUERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdPermissionQuery"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalQuiescePmaChannel__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0201u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalQuiescePmaChannel"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalSriovPromotePmaStream__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50008u)
        /*flags=*/      0x50008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0202u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalSriovPromotePmaStream"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalPermissionsInit__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0203u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalPermissionsInit"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x500c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalAllocPmaStream__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x500c8u)
        /*flags=*/      0x500c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0204u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalAllocPmaStream"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x500c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalFreePmaStream__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x500c8u)
        /*flags=*/      0x500c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0206u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalFreePmaStream"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x500c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalGetMaxPmas__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x500c8u)
        /*flags=*/      0x500c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0207u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalGetMaxPmas"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalBindPmResources__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*flags=*/      0x100c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0208u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalBindPmResources"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalUnbindPmResources__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0209u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalUnbindPmResources"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdInternalReserveHwpmLegacy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c8u)
        /*flags=*/      0x100c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc020au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalReserveHwpmLegacy"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdRequestCgControls__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0301u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdRequestCgControls"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &profilerBaseCtrlCmdReleaseCgControls__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0302u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseCgControls"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ProfilerBase __nvoc_metadata__ProfilerBase = {
    .rtti.pClassDef = &__nvoc_class_def_ProfilerBase,    // (profilerBase) this
    .rtti.dtor      = &__nvoc_dtor_ProfilerBase,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

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
const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerBase = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__ProfilerBase.rtti,    // [0]: (profilerBase) this
        &__nvoc_metadata__ProfilerBase.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerBase = 
{
    .numEntries=     35,
    .pExportEntries= __nvoc_exported_method_def_ProfilerBase
};


// Destruct ProfilerBase object.
void __nvoc_profilerBaseDestruct(ProfilerBase*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_ProfilerBase(Dynamic* pThis) {

    ProfilerBase *__nvoc_this = (ProfilerBase *) pThis;

// Call destructor.
    __nvoc_profilerBaseDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct ProfilerBase object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase *pProf, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pProf->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerBase_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_ProfilerBase(pProf, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_profilerBaseConstruct(pProf, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerBase_fail__init;
    goto __nvoc_ctor_ProfilerBase_exit; // Success

    // Unwind on error.
__nvoc_ctor_ProfilerBase_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pProf->__nvoc_base_GpuResource);
__nvoc_ctor_ProfilerBase_fail_GpuResource:
__nvoc_ctor_ProfilerBase_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ProfilerBase_1(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // profilerBaseConstructState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseConstructState__ = &profilerBaseConstructState_IMPL;
    }
    else
    {
        pThis->__profilerBaseConstructState__ = &profilerBaseConstructState_ac1694;
    }

    // profilerBaseDestructState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseDestructState__ = &profilerBaseDestructState_VF;
    }
    else
    {
        pThis->__profilerBaseDestructState__ = &profilerBaseDestructState_d44104;
    }

    // profilerBaseCtrlCmdInternalFreePmaStream -- halified (2 hals) exported (id=0xb0cc0206) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdInternalFreePmaStream__ = &profilerBaseCtrlCmdInternalFreePmaStream_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdInternalFreePmaStream__ = &profilerBaseCtrlCmdInternalFreePmaStream_ac1694;
    }

    // profilerBaseCtrlCmdUnbindPmResources -- halified (2 hals) exported (id=0xb0cc0108) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdUnbindPmResources__ = &profilerBaseCtrlCmdUnbindPmResources_395e98;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdUnbindPmResources__ = &profilerBaseCtrlCmdUnbindPmResources_IMPL;
    }

    // profilerBaseCtrlCmdPmaStreamUpdateGetPut -- halified (2 hals) exported (id=0xb0cc0109) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__ = &profilerBaseCtrlCmdPmaStreamUpdateGetPut_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__ = &profilerBaseCtrlCmdPmaStreamUpdateGetPut_ac1694;
    }

    // profilerBaseCtrlCmdInternalAllocPmaStream -- halified (2 hals) exported (id=0xb0cc0204) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdInternalAllocPmaStream__ = &profilerBaseCtrlCmdInternalAllocPmaStream_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdInternalAllocPmaStream__ = &profilerBaseCtrlCmdInternalAllocPmaStream_ac1694;
    }

    // profilerBaseCtrlCmdInternalSriovPromotePmaStream -- halified (2 hals) exported (id=0xb0cc0202) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdInternalSriovPromotePmaStream__ = &profilerBaseCtrlCmdInternalSriovPromotePmaStream_VF;
    }
    else
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }

    // profilerBaseCtrlCmdRequestCgControls -- halified (2 hals) exported (id=0xb0cc0301) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdRequestCgControls__ = &profilerBaseCtrlCmdRequestCgControls_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdRequestCgControls__ = &profilerBaseCtrlCmdRequestCgControls_92bfc3;
    }

    // profilerBaseCtrlCmdReleaseCgControls -- halified (2 hals) exported (id=0xb0cc0302) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdReleaseCgControls__ = &profilerBaseCtrlCmdReleaseCgControls_ac1694;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdReleaseCgControls__ = &profilerBaseCtrlCmdReleaseCgControls_92bfc3;
    }
} // End __nvoc_init_funcTable_ProfilerBase_1 with approximately 18 basic block(s).


// Initialize vtable(s) for 9 virtual method(s).
void __nvoc_init_funcTable_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 9 per-object function pointer(s).
    __nvoc_init_funcTable_ProfilerBase_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_ProfilerBase = pThis;    // (profilerBase) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerBase.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerBase;    // (profilerBase) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ProfilerBase(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerBase(ProfilerBase **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ProfilerBase *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ProfilerBase));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ProfilerBase));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ProfilerBase_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ProfilerBase_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ProfilerBase(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_ProfilerBase(__nvoc_pThis, pRmhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ProfilerBase_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ProfilerBase_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ProfilerBase));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerBase(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_ProfilerBase((ProfilerBase **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__e99229 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerCtx;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

// Forward declarations for ProfilerCtx
void __nvoc_init__ProfilerBase(ProfilerBase*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__ProfilerCtx(ProfilerCtx*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_ProfilerCtx(ProfilerCtx*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_ProfilerCtx(ProfilerCtx*, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_ProfilerCtx(ProfilerCtx*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_ProfilerCtx(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerCtx;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerCtx;

// Down-thunk(s) to bridge ProfilerCtx methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2

// Up-thunk(s) to bridge ProfilerCtx methods to ancestors (if any)

// Class-specific details for ProfilerCtx
const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerCtx = 
{
    .classInfo.size =               sizeof(ProfilerCtx),
    .classInfo.classId =            classId(ProfilerCtx),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ProfilerCtx",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ProfilerCtx,
    .pCastInfo =          &__nvoc_castinfo__ProfilerCtx,
    .pExportInfo =        &__nvoc_export_info__ProfilerCtx
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ProfilerCtx __nvoc_metadata__ProfilerCtx = {
    .rtti.pClassDef = &__nvoc_class_def_ProfilerCtx,    // (profilerCtx) this
    .rtti.dtor      = &__nvoc_dtor_ProfilerCtx,
    .rtti.offset    = 0,
    .metadata__ProfilerBase.rtti.pClassDef = &__nvoc_class_def_ProfilerBase,    // (profilerBase) super
    .metadata__ProfilerBase.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.rtti.offset    = NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase),
    .metadata__ProfilerBase.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__ProfilerBase.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerCtx = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__ProfilerCtx.rtti,    // [0]: (profilerCtx) this
        &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.rtti,    // [1]: (profilerBase) super
        &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerCtx = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ProfilerCtx object.
void __nvoc_profilerCtxDestruct(ProfilerCtx*);
void __nvoc_dtor_ProfilerBase(Dynamic*);
void __nvoc_dtor_ProfilerCtx(Dynamic* pThis) {

    ProfilerCtx *__nvoc_this = (ProfilerCtx *) pThis;

// Call destructor.
    __nvoc_profilerCtxDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_ProfilerBase((Dynamic *) &__nvoc_this->__nvoc_base_ProfilerBase);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct ProfilerCtx object.
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase *pProf, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_ProfilerCtx(ProfilerCtx *pResource, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_ProfilerBase(&pResource->__nvoc_base_ProfilerBase, pRmhalspecowner, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerCtx_fail_ProfilerBase;

    // Initialize data fields.
    __nvoc_init_dataField_ProfilerCtx(pResource, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_profilerCtxConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerCtx_fail__init;
    goto __nvoc_ctor_ProfilerCtx_exit; // Success

    // Unwind on error.
__nvoc_ctor_ProfilerCtx_fail__init:
    __nvoc_dtor_ProfilerBase((Dynamic *)&pResource->__nvoc_base_ProfilerBase);
__nvoc_ctor_ProfilerCtx_fail_ProfilerBase:
__nvoc_ctor_ProfilerCtx_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ProfilerCtx_1(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // profilerCtxConstructState -- halified (2 hals) override (profilerBase) base (profilerBase)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerCtxConstructState__ = &profilerCtxConstructState_VF;
    }
    else
    {
        pThis->__profilerCtxConstructState__ = &profilerCtxConstructState_IMPL;
    }

    // profilerCtxConstructStatePrologue -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerCtxConstructStatePrologue__ = &profilerCtxConstructStatePrologue_92bfc3;
    }
    else
    {
        pThis->__profilerCtxConstructStatePrologue__ = &profilerCtxConstructStatePrologue_FWCLIENT;
    }

    // profilerCtxConstructStateInterlude -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerCtxConstructStateInterlude__ = &profilerCtxConstructStateInterlude_92bfc3;
    }
    else
    {
        pThis->__profilerCtxConstructStateInterlude__ = &profilerCtxConstructStateInterlude_IMPL;
    }

    // profilerCtxConstructStateEpilogue -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerCtxConstructStateEpilogue__ = &profilerCtxConstructStateEpilogue_92bfc3;
    }
    else
    {
        pThis->__profilerCtxConstructStateEpilogue__ = &profilerCtxConstructStateEpilogue_ac1694;
    }

    // profilerCtxDestruct -- halified (2 hals) override (res) base (profilerBase) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerCtxDestruct__ = &profilerCtxDestruct_d44104;
    }
    else
    {
        pThis->__profilerCtxDestruct__ = &profilerCtxDestruct_FWCLIENT;
    }
} // End __nvoc_init_funcTable_ProfilerCtx_1 with approximately 10 basic block(s).


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 5 per-object function pointer(s).
    __nvoc_init_funcTable_ProfilerCtx_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_ProfilerBase = &pThis->__nvoc_base_ProfilerBase;    // (profilerBase) super
    pThis->__nvoc_pbase_ProfilerCtx = pThis;    // (profilerCtx) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_ProfilerBase.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx.metadata__ProfilerBase;    // (profilerBase) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerCtx;    // (profilerCtx) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ProfilerCtx(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerCtx(ProfilerCtx **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ProfilerCtx *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ProfilerCtx));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ProfilerCtx));

    __nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ProfilerCtx_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ProfilerCtx_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ProfilerCtx(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_ProfilerCtx(__nvoc_pThis, pRmhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ProfilerCtx_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ProfilerCtx_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ProfilerCtx));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerCtx(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_ProfilerCtx((ProfilerCtx **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__54d077 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

// Forward declarations for ProfilerDev
void __nvoc_init__ProfilerBase(ProfilerBase*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__ProfilerDev(ProfilerDev*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_ProfilerDev(ProfilerDev*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_ProfilerDev(ProfilerDev*, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_ProfilerDev(ProfilerDev*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_ProfilerDev(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerDev;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerDev;

// Down-thunk(s) to bridge ProfilerDev methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2

// Up-thunk(s) to bridge ProfilerDev methods to ancestors (if any)

// Class-specific details for ProfilerDev
const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev = 
{
    .classInfo.size =               sizeof(ProfilerDev),
    .classInfo.classId =            classId(ProfilerDev),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ProfilerDev",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ProfilerDev,
    .pCastInfo =          &__nvoc_castinfo__ProfilerDev,
    .pExportInfo =        &__nvoc_export_info__ProfilerDev
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ProfilerDev __nvoc_metadata__ProfilerDev = {
    .rtti.pClassDef = &__nvoc_class_def_ProfilerDev,    // (profilerDev) this
    .rtti.dtor      = &__nvoc_dtor_ProfilerDev,
    .rtti.offset    = 0,
    .metadata__ProfilerBase.rtti.pClassDef = &__nvoc_class_def_ProfilerBase,    // (profilerBase) super
    .metadata__ProfilerBase.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.rtti.offset    = NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase),
    .metadata__ProfilerBase.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__ProfilerBase.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerDev = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__ProfilerDev.rtti,    // [0]: (profilerDev) this
        &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.rtti,    // [1]: (profilerBase) super
        &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerDev = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ProfilerDev object.
void __nvoc_profilerDevDestruct(ProfilerDev*);
void __nvoc_dtor_ProfilerBase(Dynamic*);
void __nvoc_dtor_ProfilerDev(Dynamic* pThis) {

    ProfilerDev *__nvoc_this = (ProfilerDev *) pThis;

// Call destructor.
    __nvoc_profilerDevDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_ProfilerBase((Dynamic *) &__nvoc_this->__nvoc_base_ProfilerBase);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct ProfilerDev object.
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase *pProf, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_ProfilerDev(ProfilerDev *pResource, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_ProfilerBase(&pResource->__nvoc_base_ProfilerBase, pRmhalspecowner, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDev_fail_ProfilerBase;

    // Initialize data fields.
    __nvoc_init_dataField_ProfilerDev(pResource, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_profilerDevConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDev_fail__init;
    goto __nvoc_ctor_ProfilerDev_exit; // Success

    // Unwind on error.
__nvoc_ctor_ProfilerDev_fail__init:
    __nvoc_dtor_ProfilerBase((Dynamic *)&pResource->__nvoc_base_ProfilerBase);
__nvoc_ctor_ProfilerDev_fail_ProfilerBase:
__nvoc_ctor_ProfilerDev_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ProfilerDev_1(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // profilerDevConstructState -- halified (2 hals) override (profilerBase) base (profilerBase)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerDevConstructState__ = &profilerDevConstructState_VF;
    }
    else
    {
        pThis->__profilerDevConstructState__ = &profilerDevConstructState_IMPL;
    }

    // profilerDevConstructStatePrologue -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerDevConstructStatePrologue__ = &profilerDevConstructStatePrologue_92bfc3;
    }
    else
    {
        pThis->__profilerDevConstructStatePrologue__ = &profilerDevConstructStatePrologue_FWCLIENT;
    }

    // profilerDevConstructStateInterlude -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerDevConstructStateInterlude__ = &profilerDevConstructStateInterlude_92bfc3;
    }
    else
    {
        pThis->__profilerDevConstructStateInterlude__ = &profilerDevConstructStateInterlude_IMPL;
    }

    // profilerDevConstructStateEpilogue -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerDevConstructStateEpilogue__ = &profilerDevConstructStateEpilogue_92bfc3;
    }
    else
    {
        pThis->__profilerDevConstructStateEpilogue__ = &profilerDevConstructStateEpilogue_FWCLIENT;
    }

    // profilerDevDestructState -- halified (2 hals) override (profilerBase) base (profilerBase) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerDevDestructState__ = &profilerDevDestructState_d44104;
    }
    else
    {
        pThis->__profilerDevDestructState__ = &profilerDevDestructState_FWCLIENT;
    }
} // End __nvoc_init_funcTable_ProfilerDev_1 with approximately 10 basic block(s).


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 5 per-object function pointer(s).
    __nvoc_init_funcTable_ProfilerDev_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_ProfilerBase = &pThis->__nvoc_base_ProfilerBase;    // (profilerBase) super
    pThis->__nvoc_pbase_ProfilerDev = pThis;    // (profilerDev) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_ProfilerBase.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev.metadata__ProfilerBase;    // (profilerBase) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDev;    // (profilerDev) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ProfilerDev(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerDev(ProfilerDev **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ProfilerDev *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ProfilerDev));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ProfilerDev));

    __nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ProfilerDev_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ProfilerDev_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ProfilerDev(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_ProfilerDev(__nvoc_pThis, pRmhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ProfilerDev_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ProfilerDev_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ProfilerDev));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerDev(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_ProfilerDev((ProfilerDev **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

