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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveHes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0113u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveHes"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseHes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
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
    {               /*  [22] */
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
    {               /*  [23] */
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
    {               /*  [24] */
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
    {               /*  [25] */
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
    {               /*  [26] */
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
    {               /*  [27] */
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
    {               /*  [28] */
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
    {               /*  [29] */
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
    {               /*  [30] */
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
    {               /*  [31] */
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

// 25 up-thunk(s) defined to bridge methods in ProfilerBase to superclasses

// profilerBaseControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseControl(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, pParams);
}

// profilerBaseMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseMap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// profilerBaseUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseUnmap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, pCpuMapping);
}

// profilerBaseShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_profilerBaseShareCallback(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerBaseGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pGpu, pOffset, pSize);
}

// profilerBaseGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// profilerBaseInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), command, pParams, size);
}

// profilerBaseGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset));
}

// profilerBaseAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_profilerBaseAccessCallback(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// profilerBaseGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pParams);
}

// profilerBaseCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), bSubdeviceHandleProvided);
}

// profilerBaseGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), ppMemDesc);
}

// profilerBaseControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

// profilerBaseControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

// profilerBaseControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

// profilerBaseControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

// profilerBaseCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

// profilerBaseIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseIsDuplicate(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), hMemory, pDuplicate);
}

// profilerBasePreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

// profilerBaseControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pCallContext, pParams);
}

// profilerBaseIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported(struct ProfilerBase *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

// profilerBaseMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pParams);
}

// profilerBaseUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerBaseUnmapFrom(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pParams);
}

// profilerBaseGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

// profilerBaseAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerBase = 
{
    /*numEntries=*/     32,
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
        pThis->__profilerBaseCtrlCmdInternalSriovPromotePmaStream__ = &profilerBaseCtrlCmdInternalSriovPromotePmaStream_108313;
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
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdReserveHes__ = &profilerBaseCtrlCmdReserveHes_IMPL;
#endif

    // profilerBaseCtrlCmdReleaseHes -- exported (id=0xb0cc0114)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerBaseCtrlCmdReleaseHes__ = &profilerBaseCtrlCmdReleaseHes_IMPL;
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

    // profilerBaseControl -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseControl__ = &__nvoc_up_thunk_GpuResource_profilerBaseControl;

    // profilerBaseMap -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseMap__ = &__nvoc_up_thunk_GpuResource_profilerBaseMap;

    // profilerBaseUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseUnmap__ = &__nvoc_up_thunk_GpuResource_profilerBaseUnmap;

    // profilerBaseShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerBaseShareCallback;

    // profilerBaseGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize;

    // profilerBaseGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetMapAddrSpace;

    // profilerBaseInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerBaseInternalControlForward;

    // profilerBaseGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__profilerBaseGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerBaseGetInternalObjectHandle;

    // profilerBaseAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerBaseAccessCallback;

    // profilerBaseGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemInterMapParams;

    // profilerBaseCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerBaseCheckMemInterUnmap;

    // profilerBaseGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor;

    // profilerBaseControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Prologue;

    // profilerBaseControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControlSerialization_Epilogue;

    // profilerBaseControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Prologue;

    // profilerBaseControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerBaseControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerBaseControl_Epilogue;

    // profilerBaseCanCopy -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseCanCopy__ = &__nvoc_up_thunk_RsResource_profilerBaseCanCopy;

    // profilerBaseIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerBaseIsDuplicate;

    // profilerBasePreDestruct -- virtual inherited (res) base (gpures)
    pThis->__profilerBasePreDestruct__ = &__nvoc_up_thunk_RsResource_profilerBasePreDestruct;

    // profilerBaseControlFilter -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseControlFilter__ = &__nvoc_up_thunk_RsResource_profilerBaseControlFilter;

    // profilerBaseIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__profilerBaseIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerBaseIsPartialUnmapSupported;

    // profilerBaseMapTo -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseMapTo__ = &__nvoc_up_thunk_RsResource_profilerBaseMapTo;

    // profilerBaseUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerBaseUnmapFrom;

    // profilerBaseGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerBaseGetRefCount;

    // profilerBaseAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__profilerBaseAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerBaseAddAdditionalDependants;
} // End __nvoc_init_funcTable_ProfilerBase_1 with approximately 68 basic block(s).


// Initialize vtable(s) for 59 virtual method(s).
void __nvoc_init_funcTable_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 59 per-object function pointer(s).
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

// 25 up-thunk(s) defined to bridge methods in ProfilerDev to superclasses

// profilerDevControl: virtual inherited (gpures) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevControl(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, pParams);
}

// profilerDevMap: virtual inherited (gpures) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevMap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// profilerDevUnmap: virtual inherited (gpures) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevUnmap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, pCpuMapping);
}

// profilerDevShareCallback: virtual inherited (gpures) base (profilerBase)
static NvBool __nvoc_up_thunk_GpuResource_profilerDevShareCallback(struct ProfilerDev *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerDevGetRegBaseOffsetAndSize: virtual inherited (gpures) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pGpu, pOffset, pSize);
}

// profilerDevGetMapAddrSpace: virtual inherited (gpures) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevGetMapAddrSpace(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// profilerDevInternalControlForward: virtual inherited (gpures) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerDevInternalControlForward(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), command, pParams, size);
}

// profilerDevGetInternalObjectHandle: virtual inherited (gpures) base (profilerBase)
static NvHandle __nvoc_up_thunk_GpuResource_profilerDevGetInternalObjectHandle(struct ProfilerDev *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset));
}

// profilerDevAccessCallback: virtual inherited (rmres) base (profilerBase)
static NvBool __nvoc_up_thunk_RmResource_profilerDevAccessCallback(struct ProfilerDev *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// profilerDevGetMemInterMapParams: virtual inherited (rmres) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerDevGetMemInterMapParams(struct ProfilerDev *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pParams);
}

// profilerDevCheckMemInterUnmap: virtual inherited (rmres) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerDevCheckMemInterUnmap(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), bSubdeviceHandleProvided);
}

// profilerDevGetMemoryMappingDescriptor: virtual inherited (rmres) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerDevGetMemoryMappingDescriptor(struct ProfilerDev *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), ppMemDesc);
}

// profilerDevControlSerialization_Prologue: virtual inherited (rmres) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerDevControlSerialization_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

// profilerDevControlSerialization_Epilogue: virtual inherited (rmres) base (profilerBase)
static void __nvoc_up_thunk_RmResource_profilerDevControlSerialization_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

// profilerDevControl_Prologue: virtual inherited (rmres) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerDevControl_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

// profilerDevControl_Epilogue: virtual inherited (rmres) base (profilerBase)
static void __nvoc_up_thunk_RmResource_profilerDevControl_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

// profilerDevCanCopy: virtual inherited (res) base (profilerBase)
static NvBool __nvoc_up_thunk_RsResource_profilerDevCanCopy(struct ProfilerDev *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

// profilerDevIsDuplicate: virtual inherited (res) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerDevIsDuplicate(struct ProfilerDev *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), hMemory, pDuplicate);
}

// profilerDevPreDestruct: virtual inherited (res) base (profilerBase)
static void __nvoc_up_thunk_RsResource_profilerDevPreDestruct(struct ProfilerDev *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

// profilerDevControlFilter: virtual inherited (res) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerDevControlFilter(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pCallContext, pParams);
}

// profilerDevIsPartialUnmapSupported: inline virtual inherited (res) base (profilerBase) body
static NvBool __nvoc_up_thunk_RsResource_profilerDevIsPartialUnmapSupported(struct ProfilerDev *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

// profilerDevMapTo: virtual inherited (res) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerDevMapTo(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pParams);
}

// profilerDevUnmapFrom: virtual inherited (res) base (profilerBase)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerDevUnmapFrom(struct ProfilerDev *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pParams);
}

// profilerDevGetRefCount: virtual inherited (res) base (profilerBase)
static NvU32 __nvoc_up_thunk_RsResource_profilerDevGetRefCount(struct ProfilerDev *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

// profilerDevAddAdditionalDependants: virtual inherited (res) base (profilerBase)
static void __nvoc_up_thunk_RsResource_profilerDevAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pReference);
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

    // profilerDevControl -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevControl__ = &__nvoc_up_thunk_GpuResource_profilerDevControl;

    // profilerDevMap -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevMap__ = &__nvoc_up_thunk_GpuResource_profilerDevMap;

    // profilerDevUnmap -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevUnmap__ = &__nvoc_up_thunk_GpuResource_profilerDevUnmap;

    // profilerDevShareCallback -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerDevShareCallback;

    // profilerDevGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize;

    // profilerDevGetMapAddrSpace -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerDevGetMapAddrSpace;

    // profilerDevInternalControlForward -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerDevInternalControlForward;

    // profilerDevGetInternalObjectHandle -- virtual inherited (gpures) base (profilerBase)
    pThis->__profilerDevGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerDevGetInternalObjectHandle;

    // profilerDevAccessCallback -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerDevAccessCallback;

    // profilerDevGetMemInterMapParams -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerDevGetMemInterMapParams;

    // profilerDevCheckMemInterUnmap -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerDevCheckMemInterUnmap;

    // profilerDevGetMemoryMappingDescriptor -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerDevGetMemoryMappingDescriptor;

    // profilerDevControlSerialization_Prologue -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerDevControlSerialization_Prologue;

    // profilerDevControlSerialization_Epilogue -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerDevControlSerialization_Epilogue;

    // profilerDevControl_Prologue -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerDevControl_Prologue;

    // profilerDevControl_Epilogue -- virtual inherited (rmres) base (profilerBase)
    pThis->__profilerDevControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerDevControl_Epilogue;

    // profilerDevCanCopy -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevCanCopy__ = &__nvoc_up_thunk_RsResource_profilerDevCanCopy;

    // profilerDevIsDuplicate -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerDevIsDuplicate;

    // profilerDevPreDestruct -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevPreDestruct__ = &__nvoc_up_thunk_RsResource_profilerDevPreDestruct;

    // profilerDevControlFilter -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevControlFilter__ = &__nvoc_up_thunk_RsResource_profilerDevControlFilter;

    // profilerDevIsPartialUnmapSupported -- inline virtual inherited (res) base (profilerBase) body
    pThis->__profilerDevIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerDevIsPartialUnmapSupported;

    // profilerDevMapTo -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevMapTo__ = &__nvoc_up_thunk_RsResource_profilerDevMapTo;

    // profilerDevUnmapFrom -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerDevUnmapFrom;

    // profilerDevGetRefCount -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerDevGetRefCount;

    // profilerDevAddAdditionalDependants -- virtual inherited (res) base (profilerBase)
    pThis->__profilerDevAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerDevAddAdditionalDependants;
} // End __nvoc_init_funcTable_ProfilerDev_1 with approximately 35 basic block(s).


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 30 per-object function pointer(s).
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

