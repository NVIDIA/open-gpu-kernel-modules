#define NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_profiler_v2_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4976fc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_init_funcTable_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_dtor_ProfilerBase(ProfilerBase*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerBase;

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_ProfilerBase = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerBase,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ProfilerBase,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ProfilerBase = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ProfilerBase_ProfilerBase,
        &__nvoc_rtti_ProfilerBase_GpuResource,
        &__nvoc_rtti_ProfilerBase_RmResource,
        &__nvoc_rtti_ProfilerBase_RmResourceCommon,
        &__nvoc_rtti_ProfilerBase_RsResource,
        &__nvoc_rtti_ProfilerBase_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ProfilerBase),
        /*classId=*/            classId(ProfilerBase),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ProfilerBase",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ProfilerBase,
    /*pCastInfo=*/          &__nvoc_castinfo_ProfilerBase,
    /*pExportInfo=*/        &__nvoc_export_info_ProfilerBase
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ProfilerBase[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*flags=*/      0x208u,
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
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0102u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseHwpmLegacy"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReservePmAreaSmpc_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
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
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0104u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaSmpc"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdAllocPmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0105u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdAllocPmaStream"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdFreePmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0106u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdFreePmaStream"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdBindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0107u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdBindPmResources"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdUnbindPmResources_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0108u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdUnbindPmResources"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0109u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdPmaStreamUpdateGetPut"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdExecRegops_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdExecRegops"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReservePmAreaPcSampler"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaPcSampler"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetTotalHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010du,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetTotalHsCredits"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdSetHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010eu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_SET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdSetHsCredits"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010fu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetHsCredits"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveHes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0113u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveHes"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseHes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0114u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RELEASE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseHes"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetChipletHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0115u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetChipletHsCredits"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetHsCreditsMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
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
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL,
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
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL,
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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveCcuProf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0119u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_CCUPROF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveCcuProf"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseCcuProf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc011au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseCcuProf"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalQuiescePmaChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0201u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalQuiescePmaChannel"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalSriovPromotePmaStream_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40008u)
        /*flags=*/      0x40008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0202u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalSriovPromotePmaStream"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalPermissionsInit_IMPL,
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
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*flags=*/      0x400c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0204u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalAllocPmaStream"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*flags=*/      0x400c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0206u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalFreePmaStream"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalGetMaxPmas_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*flags=*/      0x400c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0207u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalGetMaxPmas"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalBindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0208u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalBindPmResources"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalUnbindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0209u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalUnbindPmResources"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc020au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalReserveHwpmLegacy"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdRequestCgControls_DISPATCH,
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
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseCgControls_DISPATCH,
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
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super

// Up-thunk(s) to bridge ProfilerBase methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseControl(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseMap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseUnmap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_profilerBaseShareCallback(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_profilerBaseAccessCallback(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseIsDuplicate(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported(struct ProfilerBase *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseUnmapFrom(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource);    // this
void __nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference);    // this

// 25 up-thunk(s) defined to bridge methods in ProfilerBase to superclasses

// profilerBaseControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseControl(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// profilerBaseMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseMap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// profilerBaseUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseUnmap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// profilerBaseShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_profilerBaseShareCallback(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerBaseGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// profilerBaseGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// profilerBaseInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)), command, pParams, size);
}

// profilerBaseGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource)));
}

// profilerBaseAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_profilerBaseAccessCallback(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// profilerBaseGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// profilerBaseCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// profilerBaseGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// profilerBaseControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerBaseControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerBaseControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerBaseControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerBaseCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerBaseIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseIsDuplicate(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// profilerBasePreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerBaseControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerBaseIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported(struct ProfilerBase *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerBaseMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerBaseUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseUnmapFrom(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerBaseGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerBaseAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerBase = 
{
    /*numEntries=*/     34,
    /*pExportEntries=*/ __nvoc_exported_method_def_ProfilerBase
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ProfilerBase(ProfilerBase *pThis) {
    __nvoc_profilerBaseDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerBase_fail_GpuResource;
    __nvoc_init_dataField_ProfilerBase(pThis, pRmhalspecowner);

    status = __nvoc_profilerBaseConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerBase_fail__init;
    goto __nvoc_ctor_ProfilerBase_exit; // Success

__nvoc_ctor_ProfilerBase_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
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
        pThis->__profilerBaseConstructState__ = &profilerBaseConstructState_56cd7a;
    }

    // profilerBaseDestructState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseDestructState__ = &profilerBaseDestructState_VF;
    }
    else
    {
        pThis->__profilerBaseDestructState__ = &profilerBaseDestructState_b3696a;
    }

    // profilerBaseCtrlCmdReserveHwpmLegacy -- exported (id=0xb0cc0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
    pThis->__profilerBaseCtrlCmdReserveHwpmLegacy__ = &profilerBaseCtrlCmdReserveHwpmLegacy_IMPL;
#endif

    // profilerBaseCtrlCmdInternalReserveHwpmLegacy -- exported (id=0xb0cc020a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__profilerBaseCtrlCmdInternalReserveHwpmLegacy__ = &profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL;
#endif

    // profilerBaseCtrlCmdReleaseHwpmLegacy -- exported (id=0xb0cc0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdReleaseHwpmLegacy__ = &profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL;
#endif

    // profilerBaseCtrlCmdReservePmAreaSmpc -- exported (id=0xb0cc0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReservePmAreaSmpc__ = &profilerBaseCtrlCmdReservePmAreaSmpc_IMPL;
#endif

    // profilerBaseCtrlCmdReleasePmAreaSmpc -- exported (id=0xb0cc0104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdReleasePmAreaSmpc__ = &profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL;
#endif

    // profilerBaseCtrlCmdAllocPmaStream -- exported (id=0xb0cc0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__profilerBaseCtrlCmdAllocPmaStream__ = &profilerBaseCtrlCmdAllocPmaStream_IMPL;
#endif

    // profilerBaseCtrlCmdFreePmaStream -- exported (id=0xb0cc0106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__profilerBaseCtrlCmdFreePmaStream__ = &profilerBaseCtrlCmdFreePmaStream_IMPL;
#endif

    // profilerBaseCtrlCmdInternalFreePmaStream -- halified (2 hals) exported (id=0xb0cc0206) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdInternalFreePmaStream__ = &profilerBaseCtrlCmdInternalFreePmaStream_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdInternalFreePmaStream__ = &profilerBaseCtrlCmdInternalFreePmaStream_56cd7a;
    }

    // profilerBaseCtrlCmdInternalGetMaxPmas -- exported (id=0xb0cc0207)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
    pThis->__profilerBaseCtrlCmdInternalGetMaxPmas__ = &profilerBaseCtrlCmdInternalGetMaxPmas_IMPL;
#endif

    // profilerBaseCtrlCmdBindPmResources -- exported (id=0xb0cc0107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__profilerBaseCtrlCmdBindPmResources__ = &profilerBaseCtrlCmdBindPmResources_IMPL;
#endif

    // profilerBaseCtrlCmdUnbindPmResources -- halified (2 hals) exported (id=0xb0cc0108) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdUnbindPmResources__ = &profilerBaseCtrlCmdUnbindPmResources_46f6a7;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdUnbindPmResources__ = &profilerBaseCtrlCmdUnbindPmResources_IMPL;
    }

    // profilerBaseCtrlCmdInternalBindPmResources -- exported (id=0xb0cc0208)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__profilerBaseCtrlCmdInternalBindPmResources__ = &profilerBaseCtrlCmdInternalBindPmResources_IMPL;
#endif

    // profilerBaseCtrlCmdInternalUnbindPmResources -- exported (id=0xb0cc0209)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__profilerBaseCtrlCmdInternalUnbindPmResources__ = &profilerBaseCtrlCmdInternalUnbindPmResources_IMPL;
#endif

    // profilerBaseCtrlCmdPmaStreamUpdateGetPut -- halified (2 hals) exported (id=0xb0cc0109) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__ = &profilerBaseCtrlCmdPmaStreamUpdateGetPut_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__ = &profilerBaseCtrlCmdPmaStreamUpdateGetPut_56cd7a;
    }

    // profilerBaseCtrlCmdExecRegops -- exported (id=0xb0cc010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdExecRegops__ = &profilerBaseCtrlCmdExecRegops_IMPL;
#endif

    // profilerBaseCtrlCmdInternalAllocPmaStream -- halified (2 hals) exported (id=0xb0cc0204) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdInternalAllocPmaStream__ = &profilerBaseCtrlCmdInternalAllocPmaStream_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdInternalAllocPmaStream__ = &profilerBaseCtrlCmdInternalAllocPmaStream_56cd7a;
    }

    // profilerBaseCtrlCmdInternalQuiescePmaChannel -- exported (id=0xb0cc0201)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdInternalQuiescePmaChannel__ = &profilerBaseCtrlCmdInternalQuiescePmaChannel_IMPL;
#endif

    // profilerBaseCtrlCmdInternalSriovPromotePmaStream -- halified (2 hals) exported (id=0xb0cc0202) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerBaseCtrlCmdInternalSriovPromotePmaStream__ = &profilerBaseCtrlCmdInternalSriovPromotePmaStream_VF;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdInternalSriovPromotePmaStream__ = &profilerBaseCtrlCmdInternalSriovPromotePmaStream_86b752;
    }

    // profilerBaseCtrlCmdInternalPermissionsInit -- exported (id=0xb0cc0203)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdInternalPermissionsInit__ = &profilerBaseCtrlCmdInternalPermissionsInit_IMPL;
#endif

    // profilerBaseCtrlCmdReservePmAreaPcSampler -- exported (id=0xb0cc010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReservePmAreaPcSampler__ = &profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL;
#endif

    // profilerBaseCtrlCmdReleasePmAreaPcSampler -- exported (id=0xb0cc010c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReleasePmAreaPcSampler__ = &profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL;
#endif

    // profilerBaseCtrlCmdGetTotalHsCredits -- exported (id=0xb0cc010d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdGetTotalHsCredits__ = &profilerBaseCtrlCmdGetTotalHsCredits_IMPL;
#endif

    // profilerBaseCtrlCmdGetChipletHsCredits -- exported (id=0xb0cc0115)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdGetChipletHsCredits__ = &profilerBaseCtrlCmdGetChipletHsCredits_IMPL;
#endif

    // profilerBaseCtrlCmdGetHsCreditsMapping -- exported (id=0xb0cc0116)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdGetHsCreditsMapping__ = &profilerBaseCtrlCmdGetHsCreditsMapping_IMPL;
#endif

    // profilerBaseCtrlCmdGetHsCredits -- exported (id=0xb0cc010f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdGetHsCredits__ = &profilerBaseCtrlCmdGetHsCredits_IMPL;
#endif

    // profilerBaseCtrlCmdSetHsCredits -- exported (id=0xb0cc010e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdSetHsCredits__ = &profilerBaseCtrlCmdSetHsCredits_IMPL;
#endif

    // profilerBaseCtrlCmdReserveHes -- exported (id=0xb0cc0113)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReserveHes__ = &profilerBaseCtrlCmdReserveHes_IMPL;
#endif

    // profilerBaseCtrlCmdReleaseHes -- exported (id=0xb0cc0114)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReleaseHes__ = &profilerBaseCtrlCmdReleaseHes_IMPL;
#endif

    // profilerBaseCtrlCmdReserveCcuProf -- exported (id=0xb0cc0119)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReserveCcuProf__ = &profilerBaseCtrlCmdReserveCcuProf_IMPL;
#endif

    // profilerBaseCtrlCmdReleaseCcuProf -- exported (id=0xb0cc011a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__profilerBaseCtrlCmdReleaseCcuProf__ = &profilerBaseCtrlCmdReleaseCcuProf_IMPL;
#endif

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
        pThis->__profilerBaseCtrlCmdReleaseCgControls__ = &profilerBaseCtrlCmdReleaseCgControls_56cd7a;
    }
    else
    {
        pThis->__profilerBaseCtrlCmdReleaseCgControls__ = &profilerBaseCtrlCmdReleaseCgControls_92bfc3;
    }

    // profilerBaseCtrlCmdDisableDynamicMMABoost -- exported (id=0xb0cc0117)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdDisableDynamicMMABoost__ = &profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL;
#endif

    // profilerBaseCtrlCmdGetDynamicMMABoostStatus -- exported (id=0xb0cc0118)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdGetDynamicMMABoostStatus__ = &profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL;
#endif
} // End __nvoc_init_funcTable_ProfilerBase_1 with approximately 45 basic block(s).


// Initialize vtable(s) for 61 virtual method(s).
void __nvoc_init_funcTable_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__ProfilerBase vtable = {
        .__profilerBaseControl__ = &__nvoc_up_thunk_GpuResource_profilerBaseControl,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__profilerBaseMap__ = &__nvoc_up_thunk_GpuResource_profilerBaseMap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__profilerBaseUnmap__ = &__nvoc_up_thunk_GpuResource_profilerBaseUnmap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__profilerBaseShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerBaseShareCallback,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__profilerBaseGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
        .__profilerBaseGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__profilerBaseInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__profilerBaseGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__profilerBaseAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerBaseAccessCallback,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__profilerBaseGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__profilerBaseCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__profilerBaseGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__profilerBaseControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__profilerBaseControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__profilerBaseControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__profilerBaseControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__profilerBaseCanCopy__ = &__nvoc_up_thunk_RsResource_profilerBaseCanCopy,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__profilerBaseIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerBaseIsDuplicate,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__profilerBasePreDestruct__ = &__nvoc_up_thunk_RsResource_profilerBasePreDestruct,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__profilerBaseControlFilter__ = &__nvoc_up_thunk_RsResource_profilerBaseControlFilter,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__profilerBaseIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__profilerBaseMapTo__ = &__nvoc_up_thunk_RsResource_profilerBaseMapTo,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__profilerBaseUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerBaseUnmapFrom,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__profilerBaseGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerBaseGetRefCount,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__profilerBaseAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.GpuResource.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.GpuResource.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_vtable = &vtable.GpuResource;    // (gpures) super
    pThis->__nvoc_vtable = &vtable;    // (profilerBase) this

    // Initialize vtable(s) with 36 per-object function pointer(s).
    __nvoc_init_funcTable_ProfilerBase_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ProfilerBase = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_ProfilerBase(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerBase(ProfilerBase **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ProfilerBase *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ProfilerBase), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ProfilerBase));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerBase);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_ProfilerBase(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ProfilerBase(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ProfilerBase_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ProfilerBase_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ProfilerBase));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerBase(ProfilerBase **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ProfilerBase(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xe99229 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerCtx;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

void __nvoc_init_ProfilerCtx(ProfilerCtx*, RmHalspecOwner* );
void __nvoc_init_funcTable_ProfilerCtx(ProfilerCtx*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ProfilerCtx(ProfilerCtx*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ProfilerCtx(ProfilerCtx*, RmHalspecOwner* );
void __nvoc_dtor_ProfilerCtx(ProfilerCtx*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerCtx;

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_ProfilerCtx = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerCtx,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ProfilerCtx,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerCtx_ProfilerBase = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerBase,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ProfilerCtx = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_ProfilerCtx_ProfilerCtx,
        &__nvoc_rtti_ProfilerCtx_ProfilerBase,
        &__nvoc_rtti_ProfilerCtx_GpuResource,
        &__nvoc_rtti_ProfilerCtx_RmResource,
        &__nvoc_rtti_ProfilerCtx_RmResourceCommon,
        &__nvoc_rtti_ProfilerCtx_RsResource,
        &__nvoc_rtti_ProfilerCtx_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerCtx = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ProfilerCtx),
        /*classId=*/            classId(ProfilerCtx),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ProfilerCtx",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ProfilerCtx,
    /*pCastInfo=*/          &__nvoc_castinfo_ProfilerCtx,
    /*pExportInfo=*/        &__nvoc_export_info_ProfilerCtx
};

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
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2

// Up-thunk(s) to bridge ProfilerCtx methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseControl(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseMap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseUnmap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_profilerBaseShareCallback(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_profilerBaseAccessCallback(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseIsDuplicate(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported(struct ProfilerBase *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseUnmapFrom(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource);    // super
void __nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxControl(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxMap(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxUnmap(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_profilerCtxShareCallback(struct ProfilerCtx *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxGetRegBaseOffsetAndSize(struct ProfilerCtx *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxGetMapAddrSpace(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxInternalControlForward(struct ProfilerCtx *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_profilerCtxGetInternalObjectHandle(struct ProfilerCtx *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_profilerCtxAccessCallback(struct ProfilerCtx *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxGetMemInterMapParams(struct ProfilerCtx *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxCheckMemInterUnmap(struct ProfilerCtx *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxGetMemoryMappingDescriptor(struct ProfilerCtx *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxControlSerialization_Prologue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerCtxControlSerialization_Epilogue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxControl_Prologue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerCtxControl_Epilogue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_profilerCtxCanCopy(struct ProfilerCtx *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxIsDuplicate(struct ProfilerCtx *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_profilerCtxPreDestruct(struct ProfilerCtx *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxControlFilter(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_profilerCtxIsPartialUnmapSupported(struct ProfilerCtx *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxMapTo(struct ProfilerCtx *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxUnmapFrom(struct ProfilerCtx *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_profilerCtxGetRefCount(struct ProfilerCtx *pResource);    // this
void __nvoc_up_thunk_RsResource_profilerCtxAddAdditionalDependants(struct RsClient *pClient, struct ProfilerCtx *pResource, RsResourceRef *pReference);    // this

// 25 up-thunk(s) defined to bridge methods in ProfilerCtx to superclasses

// profilerCtxControl: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxControl(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// profilerCtxMap: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxMap(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// profilerCtxUnmap: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxUnmap(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// profilerCtxShareCallback: virtual inherited (gpures) base (profilerBase)
NvBool __nvoc_up_thunk_GpuResource_profilerCtxShareCallback(struct ProfilerCtx *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerCtxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxGetRegBaseOffsetAndSize(struct ProfilerCtx *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// profilerCtxGetMapAddrSpace: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxGetMapAddrSpace(struct ProfilerCtx *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// profilerCtxInternalControlForward: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerCtxInternalControlForward(struct ProfilerCtx *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), command, pParams, size);
}

// profilerCtxGetInternalObjectHandle: virtual inherited (gpures) base (profilerBase)
NvHandle __nvoc_up_thunk_GpuResource_profilerCtxGetInternalObjectHandle(struct ProfilerCtx *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)));
}

// profilerCtxAccessCallback: virtual inherited (rmres) base (profilerBase)
NvBool __nvoc_up_thunk_RmResource_profilerCtxAccessCallback(struct ProfilerCtx *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// profilerCtxGetMemInterMapParams: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxGetMemInterMapParams(struct ProfilerCtx *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// profilerCtxCheckMemInterUnmap: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxCheckMemInterUnmap(struct ProfilerCtx *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// profilerCtxGetMemoryMappingDescriptor: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxGetMemoryMappingDescriptor(struct ProfilerCtx *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// profilerCtxControlSerialization_Prologue: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxControlSerialization_Prologue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerCtxControlSerialization_Epilogue: virtual inherited (rmres) base (profilerBase)
void __nvoc_up_thunk_RmResource_profilerCtxControlSerialization_Epilogue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerCtxControl_Prologue: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerCtxControl_Prologue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerCtxControl_Epilogue: virtual inherited (rmres) base (profilerBase)
void __nvoc_up_thunk_RmResource_profilerCtxControl_Epilogue(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerCtxCanCopy: virtual inherited (res) base (profilerBase)
NvBool __nvoc_up_thunk_RsResource_profilerCtxCanCopy(struct ProfilerCtx *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerCtxIsDuplicate: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxIsDuplicate(struct ProfilerCtx *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// profilerCtxPreDestruct: virtual inherited (res) base (profilerBase)
void __nvoc_up_thunk_RsResource_profilerCtxPreDestruct(struct ProfilerCtx *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerCtxControlFilter: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxControlFilter(struct ProfilerCtx *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerCtxIsPartialUnmapSupported: inline virtual inherited (res) base (profilerBase) body
NvBool __nvoc_up_thunk_RsResource_profilerCtxIsPartialUnmapSupported(struct ProfilerCtx *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerCtxMapTo: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxMapTo(struct ProfilerCtx *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerCtxUnmapFrom: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerCtxUnmapFrom(struct ProfilerCtx *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerCtxGetRefCount: virtual inherited (res) base (profilerBase)
NvU32 __nvoc_up_thunk_RsResource_profilerCtxGetRefCount(struct ProfilerCtx *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerCtxAddAdditionalDependants: virtual inherited (res) base (profilerBase)
void __nvoc_up_thunk_RsResource_profilerCtxAddAdditionalDependants(struct RsClient *pClient, struct ProfilerCtx *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerCtx, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerCtx = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ProfilerBase(ProfilerBase*);
void __nvoc_dtor_ProfilerCtx(ProfilerCtx *pThis) {
    __nvoc_profilerCtxDestruct(pThis);
    __nvoc_dtor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase* , RmHalspecOwner* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerCtx_fail_ProfilerBase;
    __nvoc_init_dataField_ProfilerCtx(pThis, pRmhalspecowner);

    status = __nvoc_profilerCtxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerCtx_fail__init;
    goto __nvoc_ctor_ProfilerCtx_exit; // Success

__nvoc_ctor_ProfilerCtx_fail__init:
    __nvoc_dtor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase);
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
        pThis->__profilerCtxConstructStateEpilogue__ = &profilerCtxConstructStateEpilogue_56cd7a;
    }

    // profilerCtxDestruct -- halified (2 hals) override (res) base (profilerBase) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__profilerCtxDestruct__ = &profilerCtxDestruct_b3696a;
    }
    else
    {
        pThis->__profilerCtxDestruct__ = &profilerCtxDestruct_FWCLIENT;
    }
} // End __nvoc_init_funcTable_ProfilerCtx_1 with approximately 8 basic block(s).


// Initialize vtable(s) for 29 virtual method(s).
void __nvoc_init_funcTable_ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__ProfilerCtx vtable = {
        .__profilerCtxControl__ = &__nvoc_up_thunk_GpuResource_profilerCtxControl,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseControl__ = &__nvoc_up_thunk_GpuResource_profilerBaseControl,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__profilerCtxMap__ = &__nvoc_up_thunk_GpuResource_profilerCtxMap,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseMap__ = &__nvoc_up_thunk_GpuResource_profilerBaseMap,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__profilerCtxUnmap__ = &__nvoc_up_thunk_GpuResource_profilerCtxUnmap,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseUnmap__ = &__nvoc_up_thunk_GpuResource_profilerBaseUnmap,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__profilerCtxShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerCtxShareCallback,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerBaseShareCallback,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__profilerCtxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerCtxGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
        .__profilerCtxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerCtxGetMapAddrSpace,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__profilerCtxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerCtxInternalControlForward,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__profilerCtxGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerCtxGetInternalObjectHandle,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__profilerCtxAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerCtxAccessCallback,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerBaseAccessCallback,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__profilerCtxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerCtxGetMemInterMapParams,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__profilerCtxCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerCtxCheckMemInterUnmap,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__profilerCtxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerCtxGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__profilerCtxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerCtxControlSerialization_Prologue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__profilerCtxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerCtxControlSerialization_Epilogue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__profilerCtxControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerCtxControl_Prologue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__profilerCtxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerCtxControl_Epilogue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__profilerCtxCanCopy__ = &__nvoc_up_thunk_RsResource_profilerCtxCanCopy,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseCanCopy__ = &__nvoc_up_thunk_RsResource_profilerBaseCanCopy,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__profilerCtxIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerCtxIsDuplicate,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerBaseIsDuplicate,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__profilerCtxPreDestruct__ = &__nvoc_up_thunk_RsResource_profilerCtxPreDestruct,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBasePreDestruct__ = &__nvoc_up_thunk_RsResource_profilerBasePreDestruct,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__profilerCtxControlFilter__ = &__nvoc_up_thunk_RsResource_profilerCtxControlFilter,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseControlFilter__ = &__nvoc_up_thunk_RsResource_profilerBaseControlFilter,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__profilerCtxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerCtxIsPartialUnmapSupported,    // inline virtual inherited (res) base (profilerBase) body
        .ProfilerBase.__profilerBaseIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .ProfilerBase.GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .ProfilerBase.GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .ProfilerBase.GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__profilerCtxMapTo__ = &__nvoc_up_thunk_RsResource_profilerCtxMapTo,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseMapTo__ = &__nvoc_up_thunk_RsResource_profilerBaseMapTo,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__profilerCtxUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerCtxUnmapFrom,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerBaseUnmapFrom,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__profilerCtxGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerCtxGetRefCount,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerBaseGetRefCount,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__profilerCtxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerCtxAddAdditionalDependants,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.ProfilerBase.GpuResource.RmResource.RsResource;    // (res) super^4
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.ProfilerBase.GpuResource.RmResource;    // (rmres) super^3
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_vtable = &vtable.ProfilerBase.GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_ProfilerBase.__nvoc_vtable = &vtable.ProfilerBase;    // (profilerBase) super
    pThis->__nvoc_vtable = &vtable;    // (profilerCtx) this

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_ProfilerCtx_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_init_ProfilerCtx(ProfilerCtx *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ProfilerCtx = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_ProfilerBase = &pThis->__nvoc_base_ProfilerBase;
    __nvoc_init_ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner);
    __nvoc_init_funcTable_ProfilerCtx(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerCtx(ProfilerCtx **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ProfilerCtx *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ProfilerCtx), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ProfilerCtx));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerCtx);

    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_ProfilerCtx(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ProfilerCtx(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ProfilerCtx_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ProfilerCtx_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ProfilerCtx));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerCtx(ProfilerCtx **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ProfilerCtx(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x54d077 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

void __nvoc_init_ProfilerDev(ProfilerDev*, RmHalspecOwner* );
void __nvoc_init_funcTable_ProfilerDev(ProfilerDev*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ProfilerDev(ProfilerDev*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ProfilerDev(ProfilerDev*, RmHalspecOwner* );
void __nvoc_dtor_ProfilerDev(ProfilerDev*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerDev;

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_ProfilerDev = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerDev,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ProfilerDev,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_ProfilerBase = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerBase,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ProfilerDev = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_ProfilerDev_ProfilerDev,
        &__nvoc_rtti_ProfilerDev_ProfilerBase,
        &__nvoc_rtti_ProfilerDev_GpuResource,
        &__nvoc_rtti_ProfilerDev_RmResource,
        &__nvoc_rtti_ProfilerDev_RmResourceCommon,
        &__nvoc_rtti_ProfilerDev_RsResource,
        &__nvoc_rtti_ProfilerDev_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ProfilerDev),
        /*classId=*/            classId(ProfilerDev),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ProfilerDev",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ProfilerDev,
    /*pCastInfo=*/          &__nvoc_castinfo_ProfilerDev,
    /*pExportInfo=*/        &__nvoc_export_info_ProfilerDev
};

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
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2

// Up-thunk(s) to bridge ProfilerDev methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseControl(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseMap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseUnmap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_profilerBaseShareCallback(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_profilerBaseAccessCallback(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseIsDuplicate(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported(struct ProfilerBase *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseUnmapFrom(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource);    // super
void __nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevControl(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevMap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevUnmap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_profilerDevShareCallback(struct ProfilerDev *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevGetMapAddrSpace(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevInternalControlForward(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_profilerDevGetInternalObjectHandle(struct ProfilerDev *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_profilerDevAccessCallback(struct ProfilerDev *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevGetMemInterMapParams(struct ProfilerDev *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevCheckMemInterUnmap(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevGetMemoryMappingDescriptor(struct ProfilerDev *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevControlSerialization_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerDevControlSerialization_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevControl_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerDevControl_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_profilerDevCanCopy(struct ProfilerDev *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevIsDuplicate(struct ProfilerDev *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_profilerDevPreDestruct(struct ProfilerDev *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevControlFilter(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_profilerDevIsPartialUnmapSupported(struct ProfilerDev *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevMapTo(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevUnmapFrom(struct ProfilerDev *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_profilerDevGetRefCount(struct ProfilerDev *pResource);    // this
void __nvoc_up_thunk_RsResource_profilerDevAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference);    // this

// 25 up-thunk(s) defined to bridge methods in ProfilerDev to superclasses

// profilerDevControl: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevControl(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// profilerDevMap: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevMap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// profilerDevUnmap: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevUnmap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// profilerDevShareCallback: virtual inherited (gpures) base (profilerBase)
NvBool __nvoc_up_thunk_GpuResource_profilerDevShareCallback(struct ProfilerDev *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerDevGetRegBaseOffsetAndSize: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// profilerDevGetMapAddrSpace: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevGetMapAddrSpace(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// profilerDevInternalControlForward: virtual inherited (gpures) base (profilerBase)
NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevInternalControlForward(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)), command, pParams, size);
}

// profilerDevGetInternalObjectHandle: virtual inherited (gpures) base (profilerBase)
NvHandle __nvoc_up_thunk_GpuResource_profilerDevGetInternalObjectHandle(struct ProfilerDev *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource)));
}

// profilerDevAccessCallback: virtual inherited (rmres) base (profilerBase)
NvBool __nvoc_up_thunk_RmResource_profilerDevAccessCallback(struct ProfilerDev *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// profilerDevGetMemInterMapParams: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevGetMemInterMapParams(struct ProfilerDev *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// profilerDevCheckMemInterUnmap: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevCheckMemInterUnmap(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// profilerDevGetMemoryMappingDescriptor: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevGetMemoryMappingDescriptor(struct ProfilerDev *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// profilerDevControlSerialization_Prologue: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevControlSerialization_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDevControlSerialization_Epilogue: virtual inherited (rmres) base (profilerBase)
void __nvoc_up_thunk_RmResource_profilerDevControlSerialization_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDevControl_Prologue: virtual inherited (rmres) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDevControl_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDevControl_Epilogue: virtual inherited (rmres) base (profilerBase)
void __nvoc_up_thunk_RmResource_profilerDevControl_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDevCanCopy: virtual inherited (res) base (profilerBase)
NvBool __nvoc_up_thunk_RsResource_profilerDevCanCopy(struct ProfilerDev *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDevIsDuplicate: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevIsDuplicate(struct ProfilerDev *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// profilerDevPreDestruct: virtual inherited (res) base (profilerBase)
void __nvoc_up_thunk_RsResource_profilerDevPreDestruct(struct ProfilerDev *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDevControlFilter: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevControlFilter(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerDevIsPartialUnmapSupported: inline virtual inherited (res) base (profilerBase) body
NvBool __nvoc_up_thunk_RsResource_profilerDevIsPartialUnmapSupported(struct ProfilerDev *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDevMapTo: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevMapTo(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerDevUnmapFrom: virtual inherited (res) base (profilerBase)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDevUnmapFrom(struct ProfilerDev *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerDevGetRefCount: virtual inherited (res) base (profilerBase)
NvU32 __nvoc_up_thunk_RsResource_profilerDevGetRefCount(struct ProfilerDev *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDevAddAdditionalDependants: virtual inherited (res) base (profilerBase)
void __nvoc_up_thunk_RsResource_profilerDevAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerDev = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ProfilerBase(ProfilerBase*);
void __nvoc_dtor_ProfilerDev(ProfilerDev *pThis) {
    __nvoc_profilerDevDestruct(pThis);
    __nvoc_dtor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase* , RmHalspecOwner* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDev_fail_ProfilerBase;
    __nvoc_init_dataField_ProfilerDev(pThis, pRmhalspecowner);

    status = __nvoc_profilerDevConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDev_fail__init;
    goto __nvoc_ctor_ProfilerDev_exit; // Success

__nvoc_ctor_ProfilerDev_fail__init:
    __nvoc_dtor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase);
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
        pThis->__profilerDevDestructState__ = &profilerDevDestructState_b3696a;
    }
    else
    {
        pThis->__profilerDevDestructState__ = &profilerDevDestructState_FWCLIENT;
    }
} // End __nvoc_init_funcTable_ProfilerDev_1 with approximately 10 basic block(s).


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__ProfilerDev vtable = {
        .__profilerDevControl__ = &__nvoc_up_thunk_GpuResource_profilerDevControl,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseControl__ = &__nvoc_up_thunk_GpuResource_profilerBaseControl,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__profilerDevMap__ = &__nvoc_up_thunk_GpuResource_profilerDevMap,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseMap__ = &__nvoc_up_thunk_GpuResource_profilerBaseMap,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__profilerDevUnmap__ = &__nvoc_up_thunk_GpuResource_profilerDevUnmap,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseUnmap__ = &__nvoc_up_thunk_GpuResource_profilerBaseUnmap,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__profilerDevShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerDevShareCallback,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerBaseShareCallback,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__profilerDevGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
        .__profilerDevGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerDevGetMapAddrSpace,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__profilerDevInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerDevInternalControlForward,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__profilerDevGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerDevGetInternalObjectHandle,    // virtual inherited (gpures) base (profilerBase)
        .ProfilerBase.__profilerBaseGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__profilerDevAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerDevAccessCallback,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerBaseAccessCallback,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__profilerDevGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerDevGetMemInterMapParams,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__profilerDevCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerDevCheckMemInterUnmap,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__profilerDevGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerDevGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__profilerDevControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerDevControlSerialization_Prologue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__profilerDevControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerDevControlSerialization_Epilogue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__profilerDevControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerDevControl_Prologue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__profilerDevControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerDevControl_Epilogue,    // virtual inherited (rmres) base (profilerBase)
        .ProfilerBase.__profilerBaseControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__profilerDevCanCopy__ = &__nvoc_up_thunk_RsResource_profilerDevCanCopy,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseCanCopy__ = &__nvoc_up_thunk_RsResource_profilerBaseCanCopy,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__profilerDevIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerDevIsDuplicate,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerBaseIsDuplicate,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__profilerDevPreDestruct__ = &__nvoc_up_thunk_RsResource_profilerDevPreDestruct,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBasePreDestruct__ = &__nvoc_up_thunk_RsResource_profilerBasePreDestruct,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__profilerDevControlFilter__ = &__nvoc_up_thunk_RsResource_profilerDevControlFilter,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseControlFilter__ = &__nvoc_up_thunk_RsResource_profilerBaseControlFilter,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__profilerDevIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerDevIsPartialUnmapSupported,    // inline virtual inherited (res) base (profilerBase) body
        .ProfilerBase.__profilerBaseIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .ProfilerBase.GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .ProfilerBase.GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .ProfilerBase.GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__profilerDevMapTo__ = &__nvoc_up_thunk_RsResource_profilerDevMapTo,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseMapTo__ = &__nvoc_up_thunk_RsResource_profilerBaseMapTo,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__profilerDevUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerDevUnmapFrom,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerBaseUnmapFrom,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__profilerDevGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerDevGetRefCount,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerBaseGetRefCount,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__profilerDevAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerDevAddAdditionalDependants,    // virtual inherited (res) base (profilerBase)
        .ProfilerBase.__profilerBaseAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .ProfilerBase.GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .ProfilerBase.GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .ProfilerBase.GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.ProfilerBase.GpuResource.RmResource.RsResource;    // (res) super^4
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.ProfilerBase.GpuResource.RmResource;    // (rmres) super^3
    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_vtable = &vtable.ProfilerBase.GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_ProfilerBase.__nvoc_vtable = &vtable.ProfilerBase;    // (profilerBase) super
    pThis->__nvoc_vtable = &vtable;    // (profilerDev) this

    // Initialize vtable(s) with 5 per-object function pointer(s).
    __nvoc_init_funcTable_ProfilerDev_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_init_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ProfilerDev = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_ProfilerBase = &pThis->__nvoc_base_ProfilerBase;
    __nvoc_init_ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner);
    __nvoc_init_funcTable_ProfilerDev(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerDev(ProfilerDev **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ProfilerDev *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ProfilerDev), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ProfilerDev));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerDev);

    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_ProfilerDev(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ProfilerDev(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ProfilerDev_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ProfilerDev_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ProfilerDev));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerDev(ProfilerDev **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ProfilerDev(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

