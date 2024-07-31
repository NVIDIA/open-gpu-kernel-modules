#define NVOC_SUBDEVICE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_subdevice_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4b01b3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_Subdevice(Subdevice*, RmHalspecOwner* );
void __nvoc_init_funcTable_Subdevice(Subdevice*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Subdevice(Subdevice*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Subdevice(Subdevice*, RmHalspecOwner* );
void __nvoc_dtor_Subdevice(Subdevice*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Subdevice;

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_Subdevice = {
    /*pClassDef=*/          &__nvoc_class_def_Subdevice,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Subdevice,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Subdevice = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_Subdevice_Subdevice,
        &__nvoc_rtti_Subdevice_Notifier,
        &__nvoc_rtti_Subdevice_INotifier,
        &__nvoc_rtti_Subdevice_GpuResource,
        &__nvoc_rtti_Subdevice_RmResource,
        &__nvoc_rtti_Subdevice_RmResourceCommon,
        &__nvoc_rtti_Subdevice_RsResource,
        &__nvoc_rtti_Subdevice_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Subdevice),
        /*classId=*/            classId(Subdevice),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Subdevice",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Subdevice,
    /*pCastInfo=*/          &__nvoc_castinfo_Subdevice,
    /*pExportInfo=*/        &__nvoc_export_info_Subdevice
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_Subdevice[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20118u)
        /*flags=*/      0x20118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInfoV2"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNameString_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010au)
        /*flags=*/      0x2010au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800110u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNameString"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetShortNameString_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50au)
        /*flags=*/      0x50au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800111u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetShortNameString"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetPower_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPower"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetSdm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SDM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSdm"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetSimulationInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*flags=*/      0x50bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800119u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSimulationInfo"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetSdm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
        /*flags=*/      0x114u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_SDM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetSdm"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuExecRegOps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuExecRegOps"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngines"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineClasslist_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineClasslist"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800125u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineFaultInfo"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800128u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryMode"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuPromoteCtx_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*flags=*/      0x10244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuPromoteCtx"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c240u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuEvictCtx_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c240u)
        /*flags=*/      0x1c240u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_EVICT_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuEvictCtx"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuInitializeCtx_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14244u)
        /*flags=*/      0x14244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuInitializeCtx"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40158u)
        /*flags=*/      0x40158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryEccStatus"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetComputeModeRules_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
        /*flags=*/      0x114u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800130u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetComputeModeRules"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800131u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryComputeModeRules"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800133u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryEccConfiguration"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*flags=*/      0x40044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800134u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetEccConfiguration"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40044u)
        /*flags=*/      0x40044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800136u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuResetEccErrorStatus"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800137u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiGpcInfo"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800138u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiTpcInfo"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800139u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiZcullInfo"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080013fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetOEMBoardInfo"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*flags=*/      0x10au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800142u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetId"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800145u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuAcquireComputeModeReservation"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800146u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuReleaseComputeModeReservation"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800147u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEnginePartnerList"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetGidInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetGidInfo"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInforomObjectVersion"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetOptimusInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetOptimusInfo"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetIpVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetIpVersion"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800153u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryIllumSupport"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetIllum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800154u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetIllum"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetIllum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800155u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetIllum"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800156u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInforomImageVersion"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800157u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryInforomEccSupport"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryScrubberStatus"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetVprCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800160u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVprCaps"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuHandleGpuSR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*flags=*/      0x3u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800167u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuHandleGpuSR"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPesInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800168u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PES_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPesInfo"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetOEMInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800169u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetOEMInfo"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetVprInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVprInfo"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEncoderCapacity"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionStats"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionInfo"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetFabricAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetFabricAddr"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEnginesV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800170u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEnginesV2"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800173u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryFunctionStatus"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetPartitions_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800174u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPartitions"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPartitions_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800175u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPartitions"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
        /*flags=*/      0x40040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800177u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuReportNonReplayableFault"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800179u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineRunlistPriBase"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetHwEngineId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetHwEngineId"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvfbcSwSessionStats"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetVmmuSegmentSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
        /*flags=*/      0x448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVmmuSegmentSize"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800181u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPartitionCapacity"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetCachedInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800182u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetCachedInfo"
#endif
    },
    {               /*  [62] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetPartitioningMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800183u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPartitioningMode"
#endif
    },
    {               /*  [63] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuDescribePartitions_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800185u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuDescribePartitions"
#endif
    },
    {               /*  [64] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800188u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetMaxSupportedPageSize"
#endif
    },
    {               /*  [65] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetActivePartitionIds"
#endif
    },
    {               /*  [66] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPids_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PIDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPids"
#endif
    },
    {               /*  [67] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPidInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPidInfo"
#endif
    },
    {               /*  [68] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuHandleVfPriFault_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800192u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuHandleVfPriFault"
#endif
    },
    {               /*  [69] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
        /*flags=*/      0x107u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800194u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetComputePolicyConfig"
#endif
    },
    {               /*  [70] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800195u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetComputePolicyConfig"
#endif
    },
    {               /*  [71] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetGfid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800196u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_GFID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetGfid"
#endif
    },
    {               /*  [72] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800197u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdUpdateGfidP2pCapability"
#endif
    },
    {               /*  [73] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdValidateMemMapRequest_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*flags=*/      0x110u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800198u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdValidateMemMapRequest"
#endif
    },
    {               /*  [74] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800199u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetEgmGpaFabricAddr"
#endif
    },
    {               /*  [75] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*flags=*/      0xau,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080019bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineLoadTimes"
#endif
    },
    {               /*  [76] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetP2pCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GET_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetP2pCaps"
#endif
    },
    {               /*  [77] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetComputeProfiles_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetComputeProfiles"
#endif
    },
    {               /*  [78] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetGpuFabricProbeInfo"
#endif
    },
    {               /*  [79] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetChipDetails_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*flags=*/      0x40448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetChipDetails"
#endif
    },
    {               /*  [80] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuMigratableOps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001a6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuMigratableOps"
#endif
    },
    {               /*  [81] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetResetStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001abu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetResetStatus"
#endif
    },
    {               /*  [82] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetDrainAndResetStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001aeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetDrainAndResetStatus"
#endif
    },
    {               /*  [83] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001afu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2"
#endif
    },
    {               /*  [84] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetConstructedFalconInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001b0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetConstructedFalconInfo"
#endif
    },
    {               /*  [85] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetVfCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001b1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVfCaps"
#endif
    },
    {               /*  [86] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGspStartTrace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
        /*flags=*/      0xd0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_GSP_START_TRACE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGspStartTrace"
#endif
    },
    {               /*  [87] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlGpuGetFipsStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208001e4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlGpuGetFipsStatus"
#endif
    },
    {               /*  [88] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetNotification_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800301u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetNotification"
#endif
    },
    {               /*  [89] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetTrigger_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800302u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetTrigger"
#endif
    },
    {               /*  [90] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetMemoryNotifies_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetMemoryNotifies"
#endif
    },
    {               /*  [91] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800304u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetSemaphoreMemory"
#endif
    },
    {               /*  [92] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetSemaMemValidation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800306u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetSemaMemValidation"
#endif
    },
    {               /*  [93] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetTriggerFifo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800308u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetTriggerFifo"
#endif
    },
    {               /*  [94] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800309u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventVideoBindEvtbuf"
#endif
    },
    {               /*  [95] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080030au,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf"
#endif
    },
    {               /*  [96] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerSchedule"
#endif
    },
    {               /*  [97] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerCancel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800402u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerCancel"
#endif
    },
    {               /*  [98] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerGetTime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800403u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetTime"
#endif
    },
    {               /*  [99] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerGetRegisterOffset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800404u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetRegisterOffset"
#endif
    },
    {               /*  [100] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800406u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo"
#endif
    },
    {               /*  [101] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerSetGrTickFreq_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*flags=*/      0x208u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800407u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerSetGrTickFreq"
#endif
    },
    {               /*  [102] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cReadBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800601u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_READ_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cReadBuffer"
#endif
    },
    {               /*  [103] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cWriteBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800602u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cWriteBuffer"
#endif
    },
    {               /*  [104] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cReadReg_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800603u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_RW_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cReadReg"
#endif
    },
    {               /*  [105] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cWriteReg_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800604u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_RW_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cWriteReg"
#endif
    },
    {               /*  [106] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*flags=*/      0xau,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800808u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetSKUInfo"
#endif
    },
    {               /*  [107] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetPostTime_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800809u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetPostTime"
#endif
    },
    {               /*  [108] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetUefiSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080080bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetUefiSupport"
#endif
    },
    {               /*  [109] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetNbsiV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080080eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetNbsiV2"
#endif
    },
    {               /*  [110] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetInfoV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800810u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetInfoV2"
#endif
    },
    {               /*  [111] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayGetStaticInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetStaticInfo"
#endif
    },
    {               /*  [112] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c0c0u)
        /*flags=*/      0x5c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysGetStaticConfig"
#endif
    },
    {               /*  [113] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer"
#endif
    },
    {               /*  [114] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer"
#endif
    },
    {               /*  [115] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetCaps"
#endif
    },
    {               /*  [116] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a22u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder"
#endif
    },
    {               /*  [117] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a26u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks"
#endif
    },
    {               /*  [118] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a28u,
        /*paramSize=*/  sizeof(NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferPtes"
#endif
    },
    {               /*  [119] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetInfo"
#endif
    },
    {               /*  [120] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetZcullInfo"
#endif
    },
    {               /*  [121] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetRopInfo"
#endif
    },
    {               /*  [122] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a30u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetPpcMasks"
#endif
    },
    {               /*  [123] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c1c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c1c0u)
        /*flags=*/      0x1c1c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a32u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo"
#endif
    },
    {               /*  [124] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a34u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier"
#endif
    },
    {               /*  [125] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x404c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetChipInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x404c0u)
        /*flags=*/      0x404c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a36u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetChipInfo"
#endif
    },
    {               /*  [126] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a37u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable"
#endif
    },
    {               /*  [127] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a38u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable"
#endif
    },
    {               /*  [128] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a39u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset"
#endif
    },
    {               /*  [129] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset"
#endif
    },
    {               /*  [130] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset"
#endif
    },
    {               /*  [131] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize"
#endif
    },
    {               /*  [132] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10040u)
        /*flags=*/      0x10040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrStaticGetFecsTraceDefines"
#endif
    },
    {               /*  [133] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines"
#endif
    },
    {               /*  [134] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c4c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c4c0u)
        /*flags=*/      0x1c4c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a40u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetDeviceInfoTable"
#endif
    },
    {               /*  [135] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*flags=*/      0x4c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a41u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetUserRegisterAccessMap"
#endif
    },
    {               /*  [136] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a44u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange"
#endif
    },
    {               /*  [137] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalInitBug4208224War_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*flags=*/      0x1d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a46u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalInitBug4208224War"
#endif
    },
    {               /*  [138] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
        /*flags=*/      0x1c0c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a48u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetPdbProperties"
#endif
    },
    {               /*  [139] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayWriteInstMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a49u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayWriteInstMem"
#endif
    },
    {               /*  [140] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRecoverAllComputeContexts"
#endif
    },
    {               /*  [141] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayGetIpVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetIpVersion"
#endif
    },
    {               /*  [142] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetSmcMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetSmcMode"
#endif
    },
    {               /*  [143] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetupRgLineIntr"
#endif
    },
    {               /*  [144] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a51u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysSetPartitionableMem"
#endif
    },
    {               /*  [145] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a53u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers"
#endif
    },
    {               /*  [146] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetImportedImpData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a54u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetImportedImpData"
#endif
    },
    {               /*  [147] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdIsEgpuBridge_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a55u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIsEgpuBridge"
#endif
    },
    {               /*  [148] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a57u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries"
#endif
    },
    {               /*  [149] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a58u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetChannelPushbuffer"
#endif
    },
    {               /*  [150] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGmmuGetStaticInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a59u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGmmuGetStaticInfo"
#endif
    },
    {               /*  [151] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetHeapReservationSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetHeapReservationSize"
#endif
    },
    {               /*  [152] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdIntrGetKernelTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIntrGetKernelTable"
#endif
    },
    {               /*  [153] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayGetDisplayMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetDisplayMask"
#endif
    },
    {               /*  [154] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
        /*flags=*/      0x1d8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a61u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoGetNumChannels"
#endif
    },
    {               /*  [155] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a63u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles"
#endif
    },
    {               /*  [156] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a65u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines"
#endif
    },
    {               /*  [157] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a66u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges"
#endif
    },
    {               /*  [158] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a67u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKMemSysGetMIGMemoryConfig"
#endif
    },
    {               /*  [159] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetZbcReferenced_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a69u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetZbcReferenced"
#endif
    },
    {               /*  [160] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRcWatchdogTimeout"
#endif
    },
    {               /*  [161] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable"
#endif
    },
    {               /*  [162] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysL2InvalidateEvict"
#endif
    },
    {               /*  [163] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6du,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches"
#endif
    },
    {               /*  [164] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6eu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysDisableNvlinkPeers"
#endif
    },
    {               /*  [165] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysProgramRawCompressionMode"
#endif
    },
    {               /*  [166] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a70u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusFlushWithSysmembar"
#endif
    },
    {               /*  [167] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a71u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal"
#endif
    },
    {               /*  [168] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a72u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote"
#endif
    },
    {               /*  [169] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a73u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusDestroyP2pMailbox"
#endif
    },
    {               /*  [170] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a74u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusCreateC2cPeerMapping"
#endif
    },
    {               /*  [171] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a75u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping"
#endif
    },
    {               /*  [172] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayPreUnixConsole_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a76u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayPreUnixConsole"
#endif
    },
    {               /*  [173] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayPostUnixConsole_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a77u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayPostUnixConsole"
#endif
    },
    {               /*  [174] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubGetMaxHshubsPerShim_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a79u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetMaxHshubsPerShim"
#endif
    },
    {               /*  [175] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfCudaLimitDisable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfCudaLimitDisable"
#endif
    },
    {               /*  [176] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit"
#endif
    },
    {               /*  [177] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfOptpCliClear_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfOptpCliClear"
#endif
    },
    {               /*  [178] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl"
#endif
    },
    {               /*  [179] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits"
#endif
    },
    {               /*  [180] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a80u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo"
#endif
    },
    {               /*  [181] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfGetAuxPowerState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a81u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGetAuxPowerState"
#endif
    },
    {               /*  [182] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubPeerConnConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a88u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubPeerConnConfig"
#endif
    },
    {               /*  [183] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubFirstLinkPeerId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a89u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubFirstLinkPeerId"
#endif
    },
    {               /*  [184] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetHshubIdForLinks"
#endif
    },
    {               /*  [185] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubGetNumUnits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetNumUnits"
#endif
    },
    {               /*  [186] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubNextHshubId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubNextHshubId"
#endif
    },
    {               /*  [187] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubEgmConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubEgmConfig"
#endif
    },
    {               /*  [188] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a98u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck"
#endif
    },
    {               /*  [189] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a99u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet"
#endif
    },
    {               /*  [190] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostSet_2x"
#endif
    },
    {               /*  [191] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer"
#endif
    },
    {               /*  [192] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer"
#endif
    },
    {               /*  [193] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer"
#endif
    },
    {               /*  [194] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer"
#endif
    },
    {               /*  [195] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer"
#endif
    },
    {               /*  [196] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostSet_3x"
#endif
    },
    {               /*  [197] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostClear_3x"
#endif
    },
    {               /*  [198] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance"
#endif
    },
    {               /*  [199] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance"
#endif
    },
    {               /*  [200] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifGetStaticInfo_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
        /*flags=*/      0x400c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aacu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifGetStaticInfo"
#endif
    },
    {               /*  [201] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aadu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr"
#endif
    },
    {               /*  [202] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aaeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr"
#endif
    },
    {               /*  [203] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifGetAspmL1Flags_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*flags=*/      0x1d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifGetAspmL1Flags"
#endif
    },
    {               /*  [204] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount"
#endif
    },
    {               /*  [205] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCcuGetSampleInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuGetSampleInfo"
#endif
    },
    {               /*  [206] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCcuMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuMap"
#endif
    },
    {               /*  [207] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCcuUnmap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuUnmap"
#endif
    },
    {               /*  [208] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalSetP2pCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSetP2pCaps"
#endif
    },
    {               /*  [209] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalRemoveP2pCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRemoveP2pCaps"
#endif
    },
    {               /*  [210] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetPcieP2pCaps"
#endif
    },
    {               /*  [211] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifSetPcieRo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifSetPcieRo"
#endif
    },
    {               /*  [212] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles"
#endif
    },
    {               /*  [213] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCcuSetStreamState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abdu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCcuSetStreamState"
#endif
    },
    {               /*  [214] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalInitGpuIntr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalInitGpuIntr"
#endif
    },
    {               /*  [215] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGsyncOptimizeTiming_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abfu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncOptimizeTiming"
#endif
    },
    {               /*  [216] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGsyncGetDisplayIds_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncGetDisplayIds"
#endif
    },
    {               /*  [217] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGsyncSetStereoSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncSetStereoSync"
#endif
    },
    {               /*  [218] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFbsrInit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFbsrInit"
#endif
    },
    {               /*  [219] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling"
#endif
    },
    {               /*  [220] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGsyncGetVactiveLines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncGetVactiveLines"
#endif
    },
    {               /*  [221] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb"
#endif
    },
    {               /*  [222] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPostInitBrightcStateLoad_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPostInitBrightcStateLoad"
#endif
    },
    {               /*  [223] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl"
#endif
    },
    {               /*  [224] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl"
#endif
    },
    {               /*  [225] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ac9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncIsDisplayIdValid"
#endif
    },
    {               /*  [226] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync"
#endif
    },
    {               /*  [227] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate"
#endif
    },
    {               /*  [228] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800accu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync"
#endif
    },
    {               /*  [229] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acdu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync"
#endif
    },
    {               /*  [230] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aceu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State"
#endif
    },
    {               /*  [231] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800acfu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State"
#endif
    },
    {               /*  [232] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit"
#endif
    },
    {               /*  [233] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit"
#endif
    },
    {               /*  [234] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode"
#endif
    },
    {               /*  [235] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2"
#endif
    },
    {               /*  [236] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad4u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo"
#endif
    },
    {               /*  [237] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping"
#endif
    },
    {               /*  [238] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate"
#endif
    },
    {               /*  [239] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFifoGetNumSecureChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoGetNumSecureChannels"
#endif
    },
    {               /*  [240] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSpdmPartition_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ad9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSpdmPartition"
#endif
    },
    {               /*  [241] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifDisableSystemMemoryAccess_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifDisableSystemMemoryAccess"
#endif
    },
    {               /*  [242] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayPinsetsToLockpins_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adcu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayPinsetsToLockpins"
#endif
    },
    {               /*  [243] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalDetectHsVideoBridge_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800addu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDetectHsVideoBridge"
#endif
    },
    {               /*  [244] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl"
#endif
    },
    {               /*  [245] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalSetStaticEdidData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800adfu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSetStaticEdidData"
#endif
    },
    {               /*  [246] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys"
#endif
    },
    {               /*  [247] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae2u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeDeriveLceKeys"
#endif
    },
    {               /*  [248] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeRotateKeys_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae5u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeRotateKeys"
#endif
    },
    {               /*  [249] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae6u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation"
#endif
    },
    {               /*  [250] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeSetGpuState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeSetGpuState"
#endif
    },
    {               /*  [251] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy"
#endif
    },
    {               /*  [252] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ae9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter"
#endif
    },
    {               /*  [253] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
        /*flags=*/      0x100c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aeau,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery"
#endif
    },
    {               /*  [254] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aebu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap"
#endif
    },
    {               /*  [255] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpuSetIllum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aecu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpuSetIllum"
#endif
    },
    {               /*  [256] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af0u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated"
#endif
    },
    {               /*  [257] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalDisplayPreModeSet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af1u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDisplayPreModeSet"
#endif
    },
    {               /*  [258] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalDisplayPostModeSet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af2u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalDisplayPostModeSet"
#endif
    },
    {               /*  [259] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalConfComputeGetStaticInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800af3u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalConfComputeGetStaticInfo"
#endif
    },
    {               /*  [260] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp"
#endif
    },
    {               /*  [261] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afbu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetLocalAtsConfig"
#endif
    },
    {               /*  [262] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afcu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalSetPeerAtsConfig"
#endif
    },
    {               /*  [263] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afdu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo"
#endif
    },
    {               /*  [264] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalInitUserSharedData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800afeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalInitUserSharedData"
#endif
    },
    {               /*  [265] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800affu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUserSharedDataSetDataPoll"
#endif
    },
    {               /*  [266] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetGpfifo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_SET_GPFIFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetGpfifo"
#endif
    },
    {               /*  [267] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoBindEngines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801103u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoBindEngines"
#endif
    },
    {               /*  [268] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetOperationalProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801104u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetOperationalProperties"
#endif
    },
    {               /*  [269] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetPhysicalChannelCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801108u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetPhysicalChannelCount"
#endif
    },
    {               /*  [270] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20008u)
        /*flags=*/      0x20008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801109u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetInfo"
#endif
    },
    {               /*  [271] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoDisableChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannels"
#endif
    },
    {               /*  [272] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetChannelMemInfo"
#endif
    },
    {               /*  [273] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetUserdLocation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetUserdLocation"
#endif
    },
    {               /*  [274] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoObjschedSwGetLog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedSwGetLog"
#endif
    },
    {               /*  [275] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5c040u)
        /*flags=*/      0x5c040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetDeviceInfoTable"
#endif
    },
    {               /*  [276] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoClearFaultedBit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*flags=*/      0x244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801113u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoClearFaultedBit"
#endif
    },
    {               /*  [277] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x68u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x68u)
        /*flags=*/      0x68u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0x20801115u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoRunlistSetSchedPolicy"
#endif
    },
    {               /*  [278] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801116u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoUpdateChannelInfo"
#endif
    },
    {               /*  [279] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801117u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableUsermodeChannels"
#endif
    },
    {               /*  [280] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb"
#endif
    },
    {               /*  [281] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801119u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetAllocatedChannels"
#endif
    },
    {               /*  [282] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080111au,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannelsForKeyRotation"
#endif
    },
    {               /*  [283] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
        /*flags=*/      0x100u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080111bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2"
#endif
    },
    {               /*  [284] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoObjschedGetState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedGetState"
#endif
    },
    {               /*  [285] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoObjschedSetState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801121u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedSetState"
#endif
    },
    {               /*  [286] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoObjschedGetCaps"
#endif
    },
    {               /*  [287] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801201u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetInfo"
#endif
    },
    {               /*  [288] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswZcullMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801205u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswZcullMode"
#endif
    },
    {               /*  [289] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetZcullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801206u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetZcullInfo"
#endif
    },
    {               /*  [290] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801207u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPmMode"
#endif
    },
    {               /*  [291] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80348u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswZcullBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80348u)
        /*flags=*/      0x80348u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801208u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswZcullBind"
#endif
    },
    {               /*  [292] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswPmBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801209u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPmBind"
#endif
    },
    {               /*  [293] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrSetGpcTileMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetGpcTileMap"
#endif
    },
    {               /*  [294] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswSmpcMode"
#endif
    },
    {               /*  [295] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmToGpcTpcMappings"
#endif
    },
    {               /*  [296] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x348u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x348u)
        /*flags=*/      0x348u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801210u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetCtxswPreemptionMode"
#endif
    },
    {               /*  [297] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801211u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPreemptionBind"
#endif
    },
    {               /*  [298] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrPcSamplingMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801212u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrPcSamplingMode"
#endif
    },
    {               /*  [299] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetROPInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801213u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ROP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetROPInfo"
#endif
    },
    {               /*  [300] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxswStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801215u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxswStats"
#endif
    },
    {               /*  [301] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801218u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferSize"
#endif
    },
    {               /*  [302] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801219u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferInfo"
#endif
    },
    {               /*  [303] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGlobalSmOrder"
#endif
    },
    {               /*  [304] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCurrentResidentChannel"
#endif
    },
    {               /*  [305] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetVatAlarmData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetVatAlarmData"
#endif
    },
    {               /*  [306] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetAttributeBufferSize"
#endif
    },
    {               /*  [307] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolQuerySize"
#endif
    },
    {               /*  [308] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801220u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolInitialize"
#endif
    },
    {               /*  [309] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801221u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolAddSlots"
#endif
    },
    {               /*  [310] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801222u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolRemoveSlots"
#endif
    },
    {               /*  [311] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*flags=*/      0x10au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801227u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCapsV2"
#endif
    },
    {               /*  [312] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801228u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetInfoV2"
#endif
    },
    {               /*  [313] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGpcMask"
#endif
    },
    {               /*  [314] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetTpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_TPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetTpcMask"
#endif
    },
    {               /*  [315] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetTpcPartitionMode"
#endif
    },
    {               /*  [316] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetEngineContextProperties"
#endif
    },
    {               /*  [317] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801230u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmIssueRateModifier"
#endif
    },
    {               /*  [318] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801231u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrFecsBindEvtbufForUid"
#endif
    },
    {               /*  [319] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
        /*flags=*/      0x14u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801232u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetPhysGpcMask"
#endif
    },
    {               /*  [320] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetPpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801233u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_PPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetPpcMask"
#endif
    },
    {               /*  [321] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801234u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetNumTpcsForGpc"
#endif
    },
    {               /*  [322] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxswModes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801235u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxswModes"
#endif
    },
    {               /*  [323] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGpcTileMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801236u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGpcTileMap"
#endif
    },
    {               /*  [324] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetZcullMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801237u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetZcullMask"
#endif
    },
    {               /*  [325] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*flags=*/      0x918u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801238u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2"
#endif
    },
    {               /*  [326] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801239u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo"
#endif
    },
    {               /*  [327] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswSetupBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080123au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswSetupBind"
#endif
    },
    {               /*  [328] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetInfoV2"
#endif
    },
    {               /*  [329] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCarveoutAddressInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCarveoutAddressInfo"
#endif
    },
    {               /*  [330] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCalibrationLockFailed"
#endif
    },
    {               /*  [331] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbFlushGpuCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbFlushGpuCache"
#endif
    },
    {               /*  [332] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetGpuCacheAllocPolicy"
#endif
    },
    {               /*  [333] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetBar1Offset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801310u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetBar1Offset"
#endif
    },
    {               /*  [334] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801312u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheAllocPolicy"
#endif
    },
    {               /*  [335] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbIsKind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801313u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_IS_KIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbIsKind"
#endif
    },
    {               /*  [336] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*flags=*/      0x40148u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801315u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheInfo"
#endif
    },
    {               /*  [337] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801318u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2"
#endif
    },
    {               /*  [338] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801319u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2"
#endif
    },
    {               /*  [339] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetFBRegionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801320u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetFBRegionInfo"
#endif
    },
    {               /*  [340] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801322u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetOfflinedPages"
#endif
    },
    {               /*  [341] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40158u)
        /*flags=*/      0x40158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801328u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetLTCInfoForFBP"
#endif
    },
    {               /*  [342] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbCBCOp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801337u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_CBC_OP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbCBCOp"
#endif
    },
    {               /*  [343] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801338u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCtagsForCbcEviction"
#endif
    },
    {               /*  [344] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetupVprRegion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetupVprRegion"
#endif
    },
    {               /*  [345] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCliManagedOfflinedPages"
#endif
    },
    {               /*  [346] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCompBitCopyConstructInfo"
#endif
    },
    {               /*  [347] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetRrd_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_RRD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetRrd"
#endif
    },
    {               /*  [348] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetReadLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetReadLimit"
#endif
    },
    {               /*  [349] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetWriteLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801340u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetWriteLimit"
#endif
    },
    {               /*  [350] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbPatchPbrForMining_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801341u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbPatchPbrForMining"
#endif
    },
    {               /*  [351] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetMemAlignment_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801342u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetMemAlignment"
#endif
    },
    {               /*  [352] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetRemappedRows_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801344u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRemappedRows"
#endif
    },
    {               /*  [353] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetFsInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801346u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_FS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetFsInfo"
#endif
    },
    {               /*  [354] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801347u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRowRemapperHistogram"
#endif
    },
    {               /*  [355] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801348u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetDynamicOfflinedPages"
#endif
    },
    {               /*  [356] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbUpdateNumaStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801350u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbUpdateNumaStatus"
#endif
    },
    {               /*  [357] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetNumaInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801351u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetNumaInfo"
#endif
    },
    {               /*  [358] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*flags=*/      0x509u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801352u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout"
#endif
    },
    {               /*  [359] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGmmuCommitTlbInvalidate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
        /*flags=*/      0x100008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801353u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGmmuCommitTlbInvalidate"
#endif
    },
    {               /*  [360] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcGetArchInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
        /*flags=*/      0x50bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801701u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetArchInfo"
#endif
    },
    {               /*  [361] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcServiceInterrupts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801702u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcServiceInterrupts"
#endif
    },
    {               /*  [362] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcGetManufacturer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801703u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetManufacturer"
#endif
    },
    {               /*  [363] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801708u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_QUERY_HOSTCLK_SLOWDOWN_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcQueryHostclkSlowdownStatus"
#endif
    },
    {               /*  [364] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcSetHostclkSlowdownStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801709u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_SET_HOSTCLK_SLOWDOWN_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcSetHostclkSlowdownStatus"
#endif
    },
    {               /*  [365] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcChangeReplayableFaultOwnership"
#endif
    },
    {               /*  [366] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170du,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetEngineNotificationIntrVectors"
#endif
    },
    {               /*  [367] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcGetStaticIntrTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetStaticIntrTable"
#endif
    },
    {               /*  [368] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPciInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
        /*flags=*/      0x518u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciInfo"
#endif
    },
    {               /*  [369] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPciBarInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
        /*flags=*/      0x518u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801803u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciBarInfo"
#endif
    },
    {               /*  [370] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801804u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieLinkWidth"
#endif
    },
    {               /*  [371] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetPcieSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801805u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieSpeed"
#endif
    },
    {               /*  [372] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801812u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusServiceGpuMultifunctionState"
#endif
    },
    {               /*  [373] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPexCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801813u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexCounters"
#endif
    },
    {               /*  [374] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusClearPexCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801814u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusClearPexCounters"
#endif
    },
    {               /*  [375] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusFreezePexCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801815u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusFreezePexCounters"
#endif
    },
    {               /*  [376] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPexLaneCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801816u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexLaneCounters"
#endif
    },
    {               /*  [377] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801817u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieLtrLatency"
#endif
    },
    {               /*  [378] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801818u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieLtrLatency"
#endif
    },
    {               /*  [379] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPexUtilCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801819u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexUtilCounters"
#endif
    },
    {               /*  [380] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusClearPexUtilCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801820u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusClearPexUtilCounters"
#endif
    },
    {               /*  [381] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetBFD_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801821u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_BFD_PARAMSARR),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetBFD"
#endif
    },
    {               /*  [382] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801822u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetAspmDisableFlags"
#endif
    },
    {               /*  [383] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20118u)
        /*flags=*/      0x20118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801823u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetInfoV2"
#endif
    },
    {               /*  [384] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusControlPublicAspmBits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801824u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusControlPublicAspmBits"
#endif
    },
    {               /*  [385] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801825u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkPeerIdMask"
#endif
    },
    {               /*  [386] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetEomParameters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801826u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetEomParameters"
#endif
    },
    {               /*  [387] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801827u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetUphyDlnCfgSpace"
#endif
    },
    {               /*  [388] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetEomStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801828u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetEomStatus"
#endif
    },
    {               /*  [389] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801829u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieReqAtomicsCaps"
#endif
    },
    {               /*  [390] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182au,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics"
#endif
    },
    {               /*  [391] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetC2CInfo_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetC2CInfo"
#endif
    },
    {               /*  [392] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSysmemAccess_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000u)
        /*flags=*/      0x1000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSysmemAccess"
#endif
    },
    {               /*  [393] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetP2pMapping_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*flags=*/      0x50040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetP2pMapping"
#endif
    },
    {               /*  [394] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50040u)
        /*flags=*/      0x50040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusUnsetP2pMapping"
#endif
    },
    {               /*  [395] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
        /*flags=*/      0x40448u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801830u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieCplAtomicsCaps"
#endif
    },
    {               /*  [396] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKPerfBoost_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
        /*flags=*/      0x318u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080200au,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_BOOST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKPerfBoost"
#endif
    },
    {               /*  [397] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetLevelInfo_V2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080200bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetLevelInfo_V2"
#endif
    },
    {               /*  [398] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetPowerstate_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080205au,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetPowerstate"
#endif
    },
    {               /*  [399] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfSetPowerstate_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080205bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfSetPowerstate"
#endif
    },
    {               /*  [400] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfNotifyVideoevent_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080205du,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfNotifyVideoevent"
#endif
    },
    {               /*  [401] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetCurrentPstate_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802068u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetCurrentPstate"
#endif
    },
    {               /*  [402] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*flags=*/      0x4au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080206eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfRatedTdpGetControl"
#endif
    },
    {               /*  [403] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfRatedTdpSetControl_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080206fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfRatedTdpSetControl"
#endif
    },
    {               /*  [404] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802087u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample"
#endif
    },
    {               /*  [405] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfSetAuxPowerState_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802092u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfSetAuxPowerState"
#endif
    },
    {               /*  [406] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802093u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfReservePerfmonHw"
#endif
    },
    {               /*  [407] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802096u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2"
#endif
    },
    {               /*  [408] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcReadVirtualMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802204u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcReadVirtualMem"
#endif
    },
    {               /*  [409] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcGetErrorCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802205u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetErrorCount"
#endif
    },
    {               /*  [410] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802207u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcSetCleanErrorHistory"
#endif
    },
    {               /*  [411] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcGetWatchdogInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802209u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetWatchdogInfo"
#endif
    },
    {               /*  [412] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcDisableWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcDisableWatchdog"
#endif
    },
    {               /*  [413] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcEnableWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcEnableWatchdog"
#endif
    },
    {               /*  [414] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcReleaseWatchdogRequests"
#endif
    },
    {               /*  [415] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetRcRecovery_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*flags=*/      0x40154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcRecovery"
#endif
    },
    {               /*  [416] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetRcRecovery_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40154u)
        /*flags=*/      0x40154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetRcRecovery"
#endif
    },
    {               /*  [417] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
        /*flags=*/      0x18u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802210u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcSoftDisableWatchdog"
#endif
    },
    {               /*  [418] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetRcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802211u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcInfo"
#endif
    },
    {               /*  [419] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetRcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802212u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetRcInfo"
#endif
    },
    {               /*  [420] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcGetErrorV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802213u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_ERROR_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetErrorV2"
#endif
    },
    {               /*  [421] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpioProgramDirection_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802300u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioProgramDirection"
#endif
    },
    {               /*  [422] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpioProgramOutput_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802301u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioProgramOutput"
#endif
    },
    {               /*  [423] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpioReadInput_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802302u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioReadInput"
#endif
    },
    {               /*  [424] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGpioActivateHwFunction_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGpioActivateHwFunction"
#endif
    },
    {               /*  [425] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdGetDumpSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetDumpSize"
#endif
    },
    {               /*  [426] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdGetDump_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802402u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetDump"
#endif
    },
    {               /*  [427] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802409u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetNocatJournalRpt"
#endif
    },
    {               /*  [428] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdSetNocatJournalData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080240bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdSetNocatJournalData"
#endif
    },
    {               /*  [429] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080240cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdInsertNocatJournalRecord"
#endif
    },
    {               /*  [430] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDmaInvalidateTLB_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802502u,
        /*paramSize=*/  sizeof(NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDmaInvalidateTLB"
#endif
    },
    {               /*  [431] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDmaGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802503u,
        /*paramSize=*/  sizeof(NV2080_CTRL_DMA_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDmaGetInfo"
#endif
    },
    {               /*  [432] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPmgrGetModuleInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802609u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PMGR_MODULE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPmgrGetModuleInfo"
#endif
    },
    {               /*  [433] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuProcessPostGc6ExitTasks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080270cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuProcessPostGc6ExitTasks"
#endif
    },
    {               /*  [434] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGc6Entry_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*flags=*/      0x2u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080270du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GC6_ENTRY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGc6Entry"
#endif
    },
    {               /*  [435] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGc6Exit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
        /*flags=*/      0x2u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080270eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GC6_EXIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGc6Exit"
#endif
    },
    {               /*  [436] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdLpwrDifrCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdLpwrDifrCtrl"
#endif
    },
    {               /*  [437] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdLpwrDifrPrefetchResponse_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802802u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdLpwrDifrPrefetchResponse"
#endif
    },
    {               /*  [438] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCaps"
#endif
    },
    {               /*  [439] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x349u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetCePceMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x349u)
        /*flags=*/      0x349u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a02u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCePceMask"
#endif
    },
    {               /*  [440] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetCapsV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCapsV2"
#endif
    },
    {               /*  [441] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeUpdatePceLceMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a05u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdatePceLceMappings"
#endif
    },
    {               /*  [442] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeUpdateClassDB_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a06u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdateClassDB"
#endif
    },
    {               /*  [443] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetPhysicalCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
        /*flags=*/      0x101d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetPhysicalCaps"
#endif
    },
    {               /*  [444] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c040u)
        /*flags=*/      0x1c040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a08u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetFaultMethodBufferSize"
#endif
    },
    {               /*  [445] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetHubPceMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
        /*flags=*/      0x4c0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a09u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetHubPceMask"
#endif
    },
    {               /*  [446] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetAllCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0au,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetAllCaps"
#endif
    },
    {               /*  [447] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
        /*flags=*/      0x1d0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetAllPhysicalCaps"
#endif
    },
    {               /*  [448] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x145u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetLceShimInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x145u)
        /*flags=*/      0x145u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetLceShimInfo"
#endif
    },
    {               /*  [449] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeUpdatePceLceMappingsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdatePceLceMappingsV2"
#endif
    },
    {               /*  [450] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetHubPceMaskV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetHubPceMaskV2"
#endif
    },
    {               /*  [451] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetPceConfigForLceType_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetPceConfigForLceType"
#endif
    },
    {               /*  [452] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetDecompLceMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
        /*flags=*/      0x154u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a11u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetDecompLceMask"
#endif
    },
    {               /*  [453] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803001u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkCaps"
#endif
    },
    {               /*  [454] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803002u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkStatus"
#endif
    },
    {               /*  [455] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkErrInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803003u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkErrInfo"
#endif
    },
    {               /*  [456] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetNvlinkCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803004u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkCounters"
#endif
    },
    {               /*  [457] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdClearNvlinkCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803005u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdClearNvlinkCounters"
#endif
    },
    {               /*  [458] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803009u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts"
#endif
    },
    {               /*  [459] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetupEom_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080300cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetupEom"
#endif
    },
    {               /*  [460] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetPowerState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080300eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetPowerState"
#endif
    },
    {               /*  [461] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinGetLinkFomValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803011u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinGetLinkFomValues"
#endif
    },
    {               /*  [462] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803014u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetNvlinkEccErrors"
#endif
    },
    {               /*  [463] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkReadTpCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803015u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkReadTpCounters"
#endif
    },
    {               /*  [464] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableNvlinkPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803017u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableNvlinkPeer"
#endif
    },
    {               /*  [465] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLpCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803018u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLpCounters"
#endif
    },
    {               /*  [466] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkCoreCallback_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803019u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CORE_CALLBACK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkCoreCallback"
#endif
    },
    {               /*  [467] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetAliEnabled_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301au,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_ALI_ENABLED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetAliEnabled"
#endif
    },
    {               /*  [468] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid"
#endif
    },
    {               /*  [469] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateHshubMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateHshubMux"
#endif
    },
    {               /*  [470] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer"
#endif
    },
    {               /*  [471] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer"
#endif
    },
    {               /*  [472] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkRemoveNvlinkMapping"
#endif
    },
    {               /*  [473] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSaveRestoreHshubState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803020u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSaveRestoreHshubState"
#endif
    },
    {               /*  [474] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProgramBufferready_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803021u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProgramBufferready"
#endif
    },
    {               /*  [475] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateCurrentConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803022u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateCurrentConfig"
#endif
    },
    {               /*  [476] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803023u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetLoopbackMode"
#endif
    },
    {               /*  [477] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803024u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdatePeerLinkMask"
#endif
    },
    {               /*  [478] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateLinkConnection_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803025u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateLinkConnection"
#endif
    },
    {               /*  [479] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableLinksPostTopology_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803026u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableLinksPostTopology"
#endif
    },
    {               /*  [480] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPreLinkTrainAli_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803027u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPreLinkTrainAli"
#endif
    },
    {               /*  [481] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803028u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetRefreshCounters"
#endif
    },
    {               /*  [482] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803029u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkClearRefreshCounters"
#endif
    },
    {               /*  [483] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
        /*flags=*/      0x10041u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302au,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet"
#endif
    },
    {               /*  [484] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkLinkTrainAli_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_LINK_TRAIN_ALI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkLinkTrainAli"
#endif
    },
    {               /*  [485] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo"
#endif
    },
    {               /*  [486] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo"
#endif
    },
    {               /*  [487] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProgramLinkSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROGRAM_LINK_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProgramLinkSpeed"
#endif
    },
    {               /*  [488] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkAreLinksTrained_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkAreLinksTrained"
#endif
    },
    {               /*  [489] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkResetLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803030u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_RESET_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkResetLinks"
#endif
    },
    {               /*  [490] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkDisableDlInterrupts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803031u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkDisableDlInterrupts"
#endif
    },
    {               /*  [491] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803032u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkAndClockInfo"
#endif
    },
    {               /*  [492] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803033u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetupNvlinkSysmem"
#endif
    },
    {               /*  [493] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProcessForcedConfigs_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803034u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProcessForcedConfigs"
#endif
    },
    {               /*  [494] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803035u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSyncLaneShutdownProps"
#endif
    },
    {               /*  [495] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803036u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts"
#endif
    },
    {               /*  [496] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
        /*flags=*/      0x41u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803037u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask"
#endif
    },
    {               /*  [497] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803038u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr"
#endif
    },
    {               /*  [498] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
        /*flags=*/      0x10041u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803039u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo"
#endif
    },
    {               /*  [499] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableLinks"
#endif
    },
    {               /*  [500] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProcessInitDisabledLinks"
#endif
    },
    {               /*  [501] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEomControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEomControl"
#endif
    },
    {               /*  [502] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetL1Threshold_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetL1Threshold"
#endif
    },
    {               /*  [503] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetL1Threshold_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetL1Threshold"
#endif
    },
    {               /*  [504] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10250u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkInbandSendData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10250u)
        /*flags=*/      0x10250u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803040u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkInbandSendData"
#endif
    },
    {               /*  [505] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkDirectConnectCheck_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803042u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkDirectConnectCheck"
#endif
    },
    {               /*  [506] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPostFaultUp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803043u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPostFaultUp"
#endif
    },
    {               /*  [507] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetPortEvents_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803044u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetPortEvents"
#endif
    },
    {               /*  [508] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdIsNvlinkReducedConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803046u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIsNvlinkReducedConfig"
#endif
    },
    {               /*  [509] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPAOS_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803047u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PAOS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPAOS"
#endif
    },
    {               /*  [510] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetNvlinkCountersV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803050u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkCountersV2"
#endif
    },
    {               /*  [511] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdClearNvlinkCountersV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803051u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdClearNvlinkCountersV2"
#endif
    },
    {               /*  [512] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkClearLpCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803052u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkClearLpCounters"
#endif
    },
    {               /*  [513] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPLTC_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803053u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PLTC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPLTC"
#endif
    },
    {               /*  [514] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPLM_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803054u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPLM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPLM"
#endif
    },
    {               /*  [515] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPSLC_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803055u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPSLC"
#endif
    },
    {               /*  [516] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMCAM_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803056u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MCAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMCAM"
#endif
    },
    {               /*  [517] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTECR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080305cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTECR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTECR"
#endif
    },
    {               /*  [518] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTWE_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080305du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTWE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTWE"
#endif
    },
    {               /*  [519] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTEWE_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080305eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTEWE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTEWE"
#endif
    },
    {               /*  [520] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTSDE_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080305fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTSDE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTSDE"
#endif
    },
    {               /*  [521] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTCAP_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803061u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTCAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTCAP"
#endif
    },
    {               /*  [522] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPMTU_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803062u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PMTU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPMTU"
#endif
    },
    {               /*  [523] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMCIA_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803063u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MCIA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMCIA"
#endif
    },
    {               /*  [524] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPMLP_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803064u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PMLP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPMLP"
#endif
    },
    {               /*  [525] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessGHPKT_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803065u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_GHPKT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessGHPKT"
#endif
    },
    {               /*  [526] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPDDR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803066u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PDDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPDDR"
#endif
    },
    {               /*  [527] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPTT_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803068u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPTT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPTT"
#endif
    },
    {               /*  [528] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPCNT_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803069u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPCNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPCNT"
#endif
    },
    {               /*  [529] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMGIR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080306au,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MGIR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMGIR"
#endif
    },
    {               /*  [530] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPAOS_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080306bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPAOS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPAOS"
#endif
    },
    {               /*  [531] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPHCR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080306cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPHCR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPHCR"
#endif
    },
    {               /*  [532] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessSLTP_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080306du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_SLTP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessSLTP"
#endif
    },
    {               /*  [533] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPGUID_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080306eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PGUID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPGUID"
#endif
    },
    {               /*  [534] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPRT_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080306fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPRT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPRT"
#endif
    },
    {               /*  [535] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPTYS_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803070u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PTYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPTYS"
#endif
    },
    {               /*  [536] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessSLRG_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803071u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_SLRG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessSLRG"
#endif
    },
    {               /*  [537] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPMAOS_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803072u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PMAOS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPMAOS"
#endif
    },
    {               /*  [538] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPLR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803073u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPLR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPLR"
#endif
    },
    {               /*  [539] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetSupportedCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803074u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetSupportedCounters"
#endif
    },
    {               /*  [540] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMORD_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803075u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MORD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMORD"
#endif
    },
    {               /*  [541] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803076u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP"
#endif
    },
    {               /*  [542] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803077u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CONF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF"
#endif
    },
    {               /*  [543] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803078u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL"
#endif
    },
    {               /*  [544] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTEIM_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803079u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTEIM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTEIM"
#endif
    },
    {               /*  [545] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTIE_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080307au,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTIE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTIE"
#endif
    },
    {               /*  [546] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTIM_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080307bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTIM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTIM"
#endif
    },
    {               /*  [547] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMPSCR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080307cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MPSCR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMPSCR"
#endif
    },
    {               /*  [548] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMTSR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080307du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MTSR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMTSR"
#endif
    },
    {               /*  [549] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPPSLS_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080307eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPPSLS"
#endif
    },
    {               /*  [550] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessMLPC_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080307fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_MLPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessMLPC"
#endif
    },
    {               /*  [551] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPRMAccessPLIB_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803080u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRM_ACCESS_PLIB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPRMAccessPLIB"
#endif
    },
    {               /*  [552] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803081u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings"
#endif
    },
    {               /*  [553] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803082u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings"
#endif
    },
    {               /*  [554] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetDmemUsage_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803101u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetDmemUsage"
#endif
    },
    {               /*  [555] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetEngineArch_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetEngineArch"
#endif
    },
    {               /*  [556] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerQueueInfo"
#endif
    },
    {               /*  [557] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerControlGet"
#endif
    },
    {               /*  [558] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerControlSet"
#endif
    },
    {               /*  [559] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetCtxBufferInfo"
#endif
    },
    {               /*  [560] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803125u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetCtxBufferSize"
#endif
    },
    {               /*  [561] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEccGetClientExposedCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803400u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetClientExposedCounters"
#endif
    },
    {               /*  [562] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEccGetEciCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_ECI_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetEciCounters"
#endif
    },
    {               /*  [563] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEccGetVolatileCounts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803402u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetVolatileCounts"
#endif
    },
    {               /*  [564] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaRange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803501u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaRange"
#endif
    },
    {               /*  [565] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
        /*flags=*/      0x10244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803502u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaSetupInstanceMemBlock"
#endif
    },
    {               /*  [566] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaGetRange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*flags=*/      0x10004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803503u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_GET_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaGetRange"
#endif
    },
    {               /*  [567] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaGetFabricMemStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803504u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaGetFabricMemStats"
#endif
    },
    {               /*  [568] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40549u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGspGetFeatures_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40549u)
        /*flags=*/      0x40549u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803601u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GSP_GET_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspGetFeatures"
#endif
    },
    {               /*  [569] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGspGetRmHeapStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803602u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspGetRmHeapStats"
#endif
    },
    {               /*  [570] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrmgrGetGrFsInfo"
#endif
    },
    {               /*  [571] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*flags=*/      0x3u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixGc6BlockerRefCnt"
#endif
    },
    {               /*  [572] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d02u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixAllowDisallowGcoff"
#endif
    },
    {               /*  [573] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*flags=*/      0x1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixAudioDynamicPower"
#endif
    },
    {               /*  [574] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xbu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xbu)
        /*flags=*/      0xbu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixVidmemPersistenceStatus"
#endif
    },
    {               /*  [575] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d08u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixUpdateTgpStatus"
#endif
    },
    {               /*  [576] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804001u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask"
#endif
    },
    {               /*  [577] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804002u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask"
#endif
    },
    {               /*  [578] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804003u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType"
#endif
    },
    {               /*  [579] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804004u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu"
#endif
    },
    {               /*  [580] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804005u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo"
#endif
    },
    {               /*  [581] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804006u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage"
#endif
    },
    {               /*  [582] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804007u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity"
#endif
    },
    {               /*  [583] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804008u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources"
#endif
    },
    {               /*  [584] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804009u,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding"
#endif
    },
    {               /*  [585] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400au,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport"
#endif
    },
    {               /*  [586] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig"
#endif
    },
    {               /*  [587] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdVgpuMgrInternalFreeStates_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080400cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdVgpuMgrInternalFreeStates"
#endif
    },
    {               /*  [588] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetAvailableHshubMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804101u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetAvailableHshubMask"
#endif
    },
    {               /*  [589] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlSetEcThrottleMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
        /*flags=*/      0x158u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlSetEcThrottleMode"
#endif
    },
    {               /*  [590] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGcxEntryPrerequisite_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080a7d7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGcxEntryPrerequisite"
#endif
    },

};

// 2 down-thunk(s) defined to bridge methods in Subdevice from superclasses

// subdevicePreDestruct: virtual override (res) base (gpures)
static void __nvoc_down_thunk_Subdevice_resPreDestruct(struct RsResource *pResource) {
    subdevicePreDestruct((struct Subdevice *)(((unsigned char *) pResource) - __nvoc_rtti_Subdevice_RsResource.offset));
}

// subdeviceInternalControlForward: virtual override (gpures) base (gpures)
static NV_STATUS __nvoc_down_thunk_Subdevice_gpuresInternalControlForward(struct GpuResource *pSubdevice, NvU32 command, void *pParams, NvU32 size) {
    return subdeviceInternalControlForward((struct Subdevice *)(((unsigned char *) pSubdevice) - __nvoc_rtti_Subdevice_GpuResource.offset), command, pParams, size);
}


// 28 up-thunk(s) defined to bridge methods in Subdevice to superclasses

// subdeviceControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceControl(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, pParams);
}

// subdeviceMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceMap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// subdeviceUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceUnmap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, pCpuMapping);
}

// subdeviceShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_subdeviceShareCallback(struct Subdevice *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// subdeviceGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize(struct Subdevice *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pGpu, pOffset, pSize);
}

// subdeviceGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_subdeviceGetMapAddrSpace(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// subdeviceGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_subdeviceGetInternalObjectHandle(struct Subdevice *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset));
}

// subdeviceAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_subdeviceAccessCallback(struct Subdevice *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// subdeviceGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_subdeviceGetMemInterMapParams(struct Subdevice *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Subdevice_RmResource.offset), pParams);
}

// subdeviceCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_subdeviceCheckMemInterUnmap(struct Subdevice *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Subdevice_RmResource.offset), bSubdeviceHandleProvided);
}

// subdeviceGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_subdeviceGetMemoryMappingDescriptor(struct Subdevice *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Subdevice_RmResource.offset), ppMemDesc);
}

// subdeviceControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_subdeviceControlSerialization_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pCallContext, pParams);
}

// subdeviceControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_subdeviceControlSerialization_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pCallContext, pParams);
}

// subdeviceControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_subdeviceControl_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pCallContext, pParams);
}

// subdeviceControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_subdeviceControl_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pCallContext, pParams);
}

// subdeviceCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_subdeviceCanCopy(struct Subdevice *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset));
}

// subdeviceIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_subdeviceIsDuplicate(struct Subdevice *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset), hMemory, pDuplicate);
}

// subdeviceControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_subdeviceControlFilter(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pCallContext, pParams);
}

// subdeviceIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_subdeviceIsPartialUnmapSupported(struct Subdevice *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset));
}

// subdeviceMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_subdeviceMapTo(struct Subdevice *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pParams);
}

// subdeviceUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_subdeviceUnmapFrom(struct Subdevice *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pParams);
}

// subdeviceGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_subdeviceGetRefCount(struct Subdevice *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset));
}

// subdeviceAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_subdeviceAddAdditionalDependants(struct RsClient *pClient, struct Subdevice *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pReference);
}

// subdeviceGetNotificationListPtr: virtual inherited (notify) base (notify)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_subdeviceGetNotificationListPtr(struct Subdevice *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset));
}

// subdeviceGetNotificationShare: virtual inherited (notify) base (notify)
static struct NotifShare * __nvoc_up_thunk_Notifier_subdeviceGetNotificationShare(struct Subdevice *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset));
}

// subdeviceSetNotificationShare: virtual inherited (notify) base (notify)
static void __nvoc_up_thunk_Notifier_subdeviceSetNotificationShare(struct Subdevice *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset), pNotifShare);
}

// subdeviceUnregisterEvent: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_subdeviceUnregisterEvent(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// subdeviceGetOrAllocNotifShare: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_subdeviceGetOrAllocNotifShare(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_Subdevice = 
{
    /*numEntries=*/     591,
    /*pExportEntries=*/ __nvoc_exported_method_def_Subdevice
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_Subdevice(Subdevice *pThis) {
    __nvoc_subdeviceDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail_Notifier;
    __nvoc_init_dataField_Subdevice(pThis, pRmhalspecowner);

    status = __nvoc_subdeviceConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail__init;
    goto __nvoc_ctor_Subdevice_exit; // Success

__nvoc_ctor_Subdevice_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_Subdevice_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_Subdevice_fail_GpuResource:
__nvoc_ctor_Subdevice_exit:

    return status;
}

// Vtable initialization 1/3
static void __nvoc_init_funcTable_Subdevice_1(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // subdevicePreDestruct -- virtual override (res) base (gpures)
    pThis->__subdevicePreDestruct__ = &subdevicePreDestruct_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__ = &__nvoc_down_thunk_Subdevice_resPreDestruct;

    // subdeviceInternalControlForward -- virtual override (gpures) base (gpures)
    pThis->__subdeviceInternalControlForward__ = &subdeviceInternalControlForward_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresInternalControlForward__ = &__nvoc_down_thunk_Subdevice_gpuresInternalControlForward;

    // subdeviceCtrlCmdBiosGetInfoV2 -- halified (2 hals) exported (id=0x20800810) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBiosGetInfoV2__ = &subdeviceCtrlCmdBiosGetInfoV2_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBiosGetInfoV2__ = &subdeviceCtrlCmdBiosGetInfoV2_92bfc3;
    }

    // subdeviceCtrlCmdBiosGetNbsiV2 -- exported (id=0x2080080e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBiosGetNbsiV2__ = &subdeviceCtrlCmdBiosGetNbsiV2_IMPL;
#endif

    // subdeviceCtrlCmdBiosGetSKUInfo -- halified (2 hals) exported (id=0x20800808)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBiosGetSKUInfo__ = &subdeviceCtrlCmdBiosGetSKUInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBiosGetSKUInfo__ = &subdeviceCtrlCmdBiosGetSKUInfo_KERNEL;
    }

    // subdeviceCtrlCmdBiosGetPostTime -- halified (2 hals) exported (id=0x20800809) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBiosGetPostTime__ = &subdeviceCtrlCmdBiosGetPostTime_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBiosGetPostTime__ = &subdeviceCtrlCmdBiosGetPostTime_92bfc3;
    }

    // subdeviceCtrlCmdBiosGetUefiSupport -- exported (id=0x2080080b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBiosGetUefiSupport__ = &subdeviceCtrlCmdBiosGetUefiSupport_IMPL;
#endif

    // subdeviceCtrlCmdMcGetArchInfo -- exported (id=0x20801701)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
    pThis->__subdeviceCtrlCmdMcGetArchInfo__ = &subdeviceCtrlCmdMcGetArchInfo_IMPL;
#endif

    // subdeviceCtrlCmdMcGetManufacturer -- exported (id=0x20801703)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdMcGetManufacturer__ = &subdeviceCtrlCmdMcGetManufacturer_IMPL;
#endif

    // subdeviceCtrlCmdMcQueryHostclkSlowdownStatus -- exported (id=0x20801708)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdMcQueryHostclkSlowdownStatus__ = &subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_IMPL;
#endif

    // subdeviceCtrlCmdMcSetHostclkSlowdownStatus -- exported (id=0x20801709)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdMcSetHostclkSlowdownStatus__ = &subdeviceCtrlCmdMcSetHostclkSlowdownStatus_IMPL;
#endif

    // subdeviceCtrlCmdMcChangeReplayableFaultOwnership -- exported (id=0x2080170c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__ = &subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL;
#endif

    // subdeviceCtrlCmdMcServiceInterrupts -- exported (id=0x20801702)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdMcServiceInterrupts__ = &subdeviceCtrlCmdMcServiceInterrupts_IMPL;
#endif

    // subdeviceCtrlCmdMcGetEngineNotificationIntrVectors -- exported (id=0x2080170d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdMcGetEngineNotificationIntrVectors__ = &subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_IMPL;
#endif

    // subdeviceCtrlCmdMcGetStaticIntrTable -- exported (id=0x2080170e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdMcGetStaticIntrTable__ = &subdeviceCtrlCmdMcGetStaticIntrTable_IMPL;
#endif

    // subdeviceCtrlCmdDmaInvalidateTLB -- exported (id=0x20802502)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdDmaInvalidateTLB__ = &subdeviceCtrlCmdDmaInvalidateTLB_IMPL;
#endif

    // subdeviceCtrlCmdDmaGetInfo -- exported (id=0x20802503)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdDmaGetInfo__ = &subdeviceCtrlCmdDmaGetInfo_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPciInfo -- exported (id=0x20801801)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
    pThis->__subdeviceCtrlCmdBusGetPciInfo__ = &subdeviceCtrlCmdBusGetPciInfo_IMPL;
#endif

    // subdeviceCtrlCmdBusGetInfoV2 -- exported (id=0x20801823)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20118u)
    pThis->__subdeviceCtrlCmdBusGetInfoV2__ = &subdeviceCtrlCmdBusGetInfoV2_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPciBarInfo -- exported (id=0x20801803)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
    pThis->__subdeviceCtrlCmdBusGetPciBarInfo__ = &subdeviceCtrlCmdBusGetPciBarInfo_IMPL;
#endif

    // subdeviceCtrlCmdBusSetPcieSpeed -- exported (id=0x20801805)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdBusSetPcieSpeed__ = &subdeviceCtrlCmdBusSetPcieSpeed_IMPL;
#endif

    // subdeviceCtrlCmdBusSetPcieLinkWidth -- exported (id=0x20801804)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdBusSetPcieLinkWidth__ = &subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL;
#endif

    // subdeviceCtrlCmdBusServiceGpuMultifunctionState -- exported (id=0x20801812)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusServiceGpuMultifunctionState__ = &subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPexCounters -- exported (id=0x20801813)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetPexCounters__ = &subdeviceCtrlCmdBusGetPexCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusGetBFD -- exported (id=0x20801821)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdBusGetBFD__ = &subdeviceCtrlCmdBusGetBFD_IMPL;
#endif

    // subdeviceCtrlCmdBusGetAspmDisableFlags -- exported (id=0x20801822)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdBusGetAspmDisableFlags__ = &subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL;
#endif

    // subdeviceCtrlCmdBusControlPublicAspmBits -- exported (id=0x20801824)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusControlPublicAspmBits__ = &subdeviceCtrlCmdBusControlPublicAspmBits_IMPL;
#endif

    // subdeviceCtrlCmdBusClearPexCounters -- exported (id=0x20801814)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusClearPexCounters__ = &subdeviceCtrlCmdBusClearPexCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPexUtilCounters -- exported (id=0x20801819)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetPexUtilCounters__ = &subdeviceCtrlCmdBusGetPexUtilCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusClearPexUtilCounters -- exported (id=0x20801820)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusClearPexUtilCounters__ = &subdeviceCtrlCmdBusClearPexUtilCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusFreezePexCounters -- exported (id=0x20801815)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusFreezePexCounters__ = &subdeviceCtrlCmdBusFreezePexCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPexLaneCounters -- exported (id=0x20801816)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetPexLaneCounters__ = &subdeviceCtrlCmdBusGetPexLaneCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPcieLtrLatency -- exported (id=0x20801817)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetPcieLtrLatency__ = &subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL;
#endif

    // subdeviceCtrlCmdBusSetPcieLtrLatency -- exported (id=0x20801818)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusSetPcieLtrLatency__ = &subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL;
#endif

    // subdeviceCtrlCmdBusGetNvlinkPeerIdMask -- exported (id=0x20801825)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__ = &subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL;
#endif

    // subdeviceCtrlCmdBusSetEomParameters -- exported (id=0x20801826)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusSetEomParameters__ = &subdeviceCtrlCmdBusSetEomParameters_IMPL;
#endif

    // subdeviceCtrlCmdBusGetUphyDlnCfgSpace -- exported (id=0x20801827)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__ = &subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL;
#endif

    // subdeviceCtrlCmdBusGetEomStatus -- exported (id=0x20801828)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetEomStatus__ = &subdeviceCtrlCmdBusGetEomStatus_IMPL;
#endif

    // subdeviceCtrlCmdBusGetPcieReqAtomicsCaps -- halified (2 hals) exported (id=0x20801829) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_92bfc3;
    }

    // subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics -- halified (2 hals) exported (id=0x2080182a) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__ = &subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__ = &subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_92bfc3;
    }

    // subdeviceCtrlCmdBusGetPcieCplAtomicsCaps -- halified (2 hals) exported (id=0x20801830) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__ = &subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_92bfc3;
    }

    // subdeviceCtrlCmdBusGetC2CInfo -- halified (2 hals) exported (id=0x2080182b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetC2CInfo__ = &subdeviceCtrlCmdBusGetC2CInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetC2CInfo__ = &subdeviceCtrlCmdBusGetC2CInfo_92bfc3;
    }

    // subdeviceCtrlCmdBusSysmemAccess -- exported (id=0x2080182c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1000u)
    pThis->__subdeviceCtrlCmdBusSysmemAccess__ = &subdeviceCtrlCmdBusSysmemAccess_IMPL;
#endif

    // subdeviceCtrlCmdBusSetP2pMapping -- halified (2 hals) exported (id=0x2080182e) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusSetP2pMapping__ = &subdeviceCtrlCmdBusSetP2pMapping_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusSetP2pMapping__ = &subdeviceCtrlCmdBusSetP2pMapping_5baef9;
    }

    // subdeviceCtrlCmdBusUnsetP2pMapping -- halified (2 hals) exported (id=0x2080182f) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusUnsetP2pMapping__ = &subdeviceCtrlCmdBusUnsetP2pMapping_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusUnsetP2pMapping__ = &subdeviceCtrlCmdBusUnsetP2pMapping_5baef9;
    }

    // subdeviceCtrlCmdNvlinkGetSupportedCounters -- exported (id=0x20803074)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdNvlinkGetSupportedCounters__ = &subdeviceCtrlCmdNvlinkGetSupportedCounters_IMPL;
#endif

    // subdeviceCtrlCmdGetNvlinkCountersV2 -- exported (id=0x20803050)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGetNvlinkCountersV2__ = &subdeviceCtrlCmdGetNvlinkCountersV2_IMPL;
#endif

    // subdeviceCtrlCmdClearNvlinkCountersV2 -- exported (id=0x20803051)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdClearNvlinkCountersV2__ = &subdeviceCtrlCmdClearNvlinkCountersV2_IMPL;
#endif

    // subdeviceCtrlCmdGetNvlinkCounters -- exported (id=0x20803004)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGetNvlinkCounters__ = &subdeviceCtrlCmdGetNvlinkCounters_IMPL;
#endif

    // subdeviceCtrlCmdClearNvlinkCounters -- exported (id=0x20803005)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdClearNvlinkCounters__ = &subdeviceCtrlCmdClearNvlinkCounters_IMPL;
#endif

    // subdeviceCtrlCmdBusGetNvlinkCaps -- halified (2 hals) exported (id=0x20803001)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBusGetNvlinkCaps__ = &subdeviceCtrlCmdBusGetNvlinkCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBusGetNvlinkCaps__ = &subdeviceCtrlCmdBusGetNvlinkCaps_IMPL;
    }

    // subdeviceCtrlCmdBusGetNvlinkStatus -- exported (id=0x20803002)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__subdeviceCtrlCmdBusGetNvlinkStatus__ = &subdeviceCtrlCmdBusGetNvlinkStatus_IMPL;
#endif

    // subdeviceCtrlCmdBusGetNvlinkErrInfo -- exported (id=0x20803003)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdBusGetNvlinkErrInfo__ = &subdeviceCtrlCmdBusGetNvlinkErrInfo_IMPL;
#endif

    // subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings -- exported (id=0x20803081)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings__ = &subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_IMPL;
#endif

    // subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings -- exported (id=0x20803082)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings__ = &subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_IMPL;
#endif

    // subdeviceCtrlCmdNvlinGetLinkFomValues -- exported (id=0x20803011)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinGetLinkFomValues__ = &subdeviceCtrlCmdNvlinGetLinkFomValues_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetNvlinkEccErrors -- exported (id=0x20803014)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkGetNvlinkEccErrors__ = &subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts -- exported (id=0x20803009)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__ = &subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSetupEom -- exported (id=0x2080300c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkSetupEom__ = &subdeviceCtrlCmdNvlinkSetupEom_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetPowerState -- exported (id=0x2080300e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdNvlinkGetPowerState__ = &subdeviceCtrlCmdNvlinkGetPowerState_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkReadTpCounters -- exported (id=0x20803015)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdNvlinkReadTpCounters__ = &subdeviceCtrlCmdNvlinkReadTpCounters_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetLpCounters -- exported (id=0x20803018)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdNvlinkGetLpCounters__ = &subdeviceCtrlCmdNvlinkGetLpCounters_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkClearLpCounters -- exported (id=0x20803052)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkClearLpCounters__ = &subdeviceCtrlCmdNvlinkClearLpCounters_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkEnableNvlinkPeer -- exported (id=0x20803017)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkEnableNvlinkPeer__ = &subdeviceCtrlCmdNvlinkEnableNvlinkPeer_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkCoreCallback -- exported (id=0x20803019)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkCoreCallback__ = &subdeviceCtrlCmdNvlinkCoreCallback_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetAliEnabled -- exported (id=0x2080301a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkGetAliEnabled__ = &subdeviceCtrlCmdNvlinkGetAliEnabled_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSetLoopbackMode -- exported (id=0x20803023)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkSetLoopbackMode__ = &subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid -- exported (id=0x2080301b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid__ = &subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkUpdateHshubMux -- exported (id=0x2080301c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateHshubMux__ = &subdeviceCtrlCmdNvlinkUpdateHshubMux_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer -- exported (id=0x2080301d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer__ = &subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer -- exported (id=0x2080301e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer__ = &subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkRemoveNvlinkMapping -- exported (id=0x2080301f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkRemoveNvlinkMapping__ = &subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSaveRestoreHshubState -- exported (id=0x20803020)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkSaveRestoreHshubState__ = &subdeviceCtrlCmdNvlinkSaveRestoreHshubState_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkProgramBufferready -- exported (id=0x20803021)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkProgramBufferready__ = &subdeviceCtrlCmdNvlinkProgramBufferready_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkUpdateCurrentConfig -- exported (id=0x20803022)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateCurrentConfig__ = &subdeviceCtrlCmdNvlinkUpdateCurrentConfig_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkUpdatePeerLinkMask -- exported (id=0x20803024)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkUpdatePeerLinkMask__ = &subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkUpdateLinkConnection -- exported (id=0x20803025)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateLinkConnection__ = &subdeviceCtrlCmdNvlinkUpdateLinkConnection_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkEnableLinksPostTopology -- exported (id=0x20803026)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkEnableLinksPostTopology__ = &subdeviceCtrlCmdNvlinkEnableLinksPostTopology_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPreLinkTrainAli -- exported (id=0x20803027)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkPreLinkTrainAli__ = &subdeviceCtrlCmdNvlinkPreLinkTrainAli_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetRefreshCounters -- exported (id=0x20803028)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkGetRefreshCounters__ = &subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkClearRefreshCounters -- exported (id=0x20803029)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkClearRefreshCounters__ = &subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet -- exported (id=0x2080302a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
    pThis->__subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet__ = &subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkLinkTrainAli -- exported (id=0x2080302b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkLinkTrainAli__ = &subdeviceCtrlCmdNvlinkLinkTrainAli_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo -- exported (id=0x2080302c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo__ = &subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo -- exported (id=0x2080302d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo__ = &subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkProgramLinkSpeed -- exported (id=0x2080302e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkProgramLinkSpeed__ = &subdeviceCtrlCmdNvlinkProgramLinkSpeed_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkAreLinksTrained -- exported (id=0x2080302f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkAreLinksTrained__ = &subdeviceCtrlCmdNvlinkAreLinksTrained_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkResetLinks -- exported (id=0x20803030)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkResetLinks__ = &subdeviceCtrlCmdNvlinkResetLinks_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkDisableDlInterrupts -- exported (id=0x20803031)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkDisableDlInterrupts__ = &subdeviceCtrlCmdNvlinkDisableDlInterrupts_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetLinkAndClockInfo -- exported (id=0x20803032)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkGetLinkAndClockInfo__ = &subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSetupNvlinkSysmem -- exported (id=0x20803033)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkSetupNvlinkSysmem__ = &subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkProcessForcedConfigs -- exported (id=0x20803034)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkProcessForcedConfigs__ = &subdeviceCtrlCmdNvlinkProcessForcedConfigs_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSyncLaneShutdownProps -- exported (id=0x20803035)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkSyncLaneShutdownProps__ = &subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts -- exported (id=0x20803036)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts__ = &subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask -- exported (id=0x20803037)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x41u)
    pThis->__subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask__ = &subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr -- exported (id=0x20803038)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__ = &subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo -- exported (id=0x20803039)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10041u)
    pThis->__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__ = &subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkEnableLinks -- exported (id=0x2080303a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkEnableLinks__ = &subdeviceCtrlCmdNvlinkEnableLinks_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkProcessInitDisabledLinks -- exported (id=0x2080303b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__ = &subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkInbandSendData -- exported (id=0x20803040)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10250u)
    pThis->__subdeviceCtrlCmdNvlinkInbandSendData__ = &subdeviceCtrlCmdNvlinkInbandSendData_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPostFaultUp -- exported (id=0x20803043)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkPostFaultUp__ = &subdeviceCtrlCmdNvlinkPostFaultUp_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkEomControl -- exported (id=0x2080303c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdNvlinkEomControl__ = &subdeviceCtrlCmdNvlinkEomControl_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkSetL1Threshold -- exported (id=0x2080303e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkSetL1Threshold__ = &subdeviceCtrlCmdNvlinkSetL1Threshold_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkDirectConnectCheck -- exported (id=0x20803042)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdNvlinkDirectConnectCheck__ = &subdeviceCtrlCmdNvlinkDirectConnectCheck_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetL1Threshold -- exported (id=0x2080303f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdNvlinkGetL1Threshold__ = &subdeviceCtrlCmdNvlinkGetL1Threshold_IMPL;
#endif

    // subdeviceCtrlCmdIsNvlinkReducedConfig -- exported (id=0x20803046)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdIsNvlinkReducedConfig__ = &subdeviceCtrlCmdIsNvlinkReducedConfig_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkGetPortEvents -- exported (id=0x20803044)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkGetPortEvents__ = &subdeviceCtrlCmdNvlinkGetPortEvents_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPAOS -- exported (id=0x20803047)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPAOS__ = &subdeviceCtrlCmdNvlinkPRMAccessPAOS_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPLTC -- exported (id=0x20803053)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPLTC__ = &subdeviceCtrlCmdNvlinkPRMAccessPLTC_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPLM -- exported (id=0x20803054)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPLM__ = &subdeviceCtrlCmdNvlinkPRMAccessPPLM_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPSLC -- exported (id=0x20803055)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPSLC__ = &subdeviceCtrlCmdNvlinkPRMAccessPPSLC_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMCAM -- exported (id=0x20803056)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMCAM__ = &subdeviceCtrlCmdNvlinkPRMAccessMCAM_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTECR -- exported (id=0x2080305c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTECR__ = &subdeviceCtrlCmdNvlinkPRMAccessMTECR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTWE -- exported (id=0x2080305d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTWE__ = &subdeviceCtrlCmdNvlinkPRMAccessMTWE_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTEWE -- exported (id=0x2080305e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTEWE__ = &subdeviceCtrlCmdNvlinkPRMAccessMTEWE_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTSDE -- exported (id=0x2080305f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTSDE__ = &subdeviceCtrlCmdNvlinkPRMAccessMTSDE_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTCAP -- exported (id=0x20803061)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTCAP__ = &subdeviceCtrlCmdNvlinkPRMAccessMTCAP_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPMTU -- exported (id=0x20803062)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPMTU__ = &subdeviceCtrlCmdNvlinkPRMAccessPMTU_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMCIA -- exported (id=0x20803063)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMCIA__ = &subdeviceCtrlCmdNvlinkPRMAccessMCIA_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPMLP -- exported (id=0x20803064)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPMLP__ = &subdeviceCtrlCmdNvlinkPRMAccessPMLP_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessGHPKT -- exported (id=0x20803065)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessGHPKT__ = &subdeviceCtrlCmdNvlinkPRMAccessGHPKT_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPDDR -- exported (id=0x20803066)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPDDR__ = &subdeviceCtrlCmdNvlinkPRMAccessPDDR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPTT -- exported (id=0x20803068)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPTT__ = &subdeviceCtrlCmdNvlinkPRMAccessPPTT_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPCNT -- exported (id=0x20803069)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPCNT__ = &subdeviceCtrlCmdNvlinkPRMAccessPPCNT_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMGIR -- exported (id=0x2080306a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMGIR__ = &subdeviceCtrlCmdNvlinkPRMAccessMGIR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPAOS -- exported (id=0x2080306b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPAOS__ = &subdeviceCtrlCmdNvlinkPRMAccessPPAOS_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPHCR -- exported (id=0x2080306c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPHCR__ = &subdeviceCtrlCmdNvlinkPRMAccessPPHCR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessSLTP -- exported (id=0x2080306d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessSLTP__ = &subdeviceCtrlCmdNvlinkPRMAccessSLTP_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPGUID -- exported (id=0x2080306e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPGUID__ = &subdeviceCtrlCmdNvlinkPRMAccessPGUID_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPRT -- exported (id=0x2080306f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPRT__ = &subdeviceCtrlCmdNvlinkPRMAccessPPRT_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPTYS -- exported (id=0x20803070)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPTYS__ = &subdeviceCtrlCmdNvlinkPRMAccessPTYS_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessSLRG -- exported (id=0x20803071)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessSLRG__ = &subdeviceCtrlCmdNvlinkPRMAccessSLRG_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPMAOS -- exported (id=0x20803072)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPMAOS__ = &subdeviceCtrlCmdNvlinkPRMAccessPMAOS_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPLR -- exported (id=0x20803073)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPLR__ = &subdeviceCtrlCmdNvlinkPRMAccessPPLR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMORD -- exported (id=0x20803075)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMORD__ = &subdeviceCtrlCmdNvlinkPRMAccessMORD_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP -- exported (id=0x20803076)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP__ = &subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF -- exported (id=0x20803077)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF__ = &subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL -- exported (id=0x20803078)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL__ = &subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTEIM -- exported (id=0x20803079)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTEIM__ = &subdeviceCtrlCmdNvlinkPRMAccessMTEIM_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTIE -- exported (id=0x2080307a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTIE__ = &subdeviceCtrlCmdNvlinkPRMAccessMTIE_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTIM -- exported (id=0x2080307b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTIM__ = &subdeviceCtrlCmdNvlinkPRMAccessMTIM_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMPSCR -- exported (id=0x2080307c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMPSCR__ = &subdeviceCtrlCmdNvlinkPRMAccessMPSCR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMTSR -- exported (id=0x2080307d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMTSR__ = &subdeviceCtrlCmdNvlinkPRMAccessMTSR_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPPSLS -- exported (id=0x2080307e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPPSLS__ = &subdeviceCtrlCmdNvlinkPRMAccessPPSLS_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessMLPC -- exported (id=0x2080307f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessMLPC__ = &subdeviceCtrlCmdNvlinkPRMAccessMLPC_IMPL;
#endif

    // subdeviceCtrlCmdNvlinkPRMAccessPLIB -- exported (id=0x20803080)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdNvlinkPRMAccessPLIB__ = &subdeviceCtrlCmdNvlinkPRMAccessPLIB_IMPL;
#endif

    // subdeviceCtrlCmdI2cReadBuffer -- exported (id=0x20800601)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdI2cReadBuffer__ = &subdeviceCtrlCmdI2cReadBuffer_IMPL;
#endif

    // subdeviceCtrlCmdI2cWriteBuffer -- exported (id=0x20800602)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdI2cWriteBuffer__ = &subdeviceCtrlCmdI2cWriteBuffer_IMPL;
#endif

    // subdeviceCtrlCmdI2cReadReg -- exported (id=0x20800603)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdI2cReadReg__ = &subdeviceCtrlCmdI2cReadReg_IMPL;
#endif

    // subdeviceCtrlCmdI2cWriteReg -- exported (id=0x20800604)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdI2cWriteReg__ = &subdeviceCtrlCmdI2cWriteReg_IMPL;
#endif

    // subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2 -- halified (2 hals) exported (id=0x20802096) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__ = &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__ = &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_5baef9;
    }

    // subdeviceCtrlCmdPerfRatedTdpGetControl -- exported (id=0x2080206e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
    pThis->__subdeviceCtrlCmdPerfRatedTdpGetControl__ = &subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL;
#endif

    // subdeviceCtrlCmdPerfRatedTdpSetControl -- halified (singleton optimized) exported (id=0x2080206f)
    pThis->__subdeviceCtrlCmdPerfRatedTdpSetControl__ = &subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL;

    // subdeviceCtrlCmdPerfReservePerfmonHw -- halified (2 hals) exported (id=0x20802093) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfReservePerfmonHw__ = &subdeviceCtrlCmdPerfReservePerfmonHw_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfReservePerfmonHw__ = &subdeviceCtrlCmdPerfReservePerfmonHw_KERNEL;
    }

    // subdeviceCtrlCmdPerfSetAuxPowerState -- halified (singleton optimized) exported (id=0x20802092)
    pThis->__subdeviceCtrlCmdPerfSetAuxPowerState__ = &subdeviceCtrlCmdPerfSetAuxPowerState_KERNEL;

    // subdeviceCtrlCmdPerfSetPowerstate -- halified (singleton optimized) exported (id=0x2080205b)
    pThis->__subdeviceCtrlCmdPerfSetPowerstate__ = &subdeviceCtrlCmdPerfSetPowerstate_KERNEL;

    // subdeviceCtrlCmdPerfGetLevelInfo_V2 -- halified (2 hals) exported (id=0x2080200b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetLevelInfo_V2__ = &subdeviceCtrlCmdPerfGetLevelInfo_V2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetLevelInfo_V2__ = &subdeviceCtrlCmdPerfGetLevelInfo_V2_92bfc3;
    }

    // subdeviceCtrlCmdPerfGetCurrentPstate -- halified (2 hals) exported (id=0x20802068) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetCurrentPstate__ = &subdeviceCtrlCmdPerfGetCurrentPstate_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetCurrentPstate__ = &subdeviceCtrlCmdPerfGetCurrentPstate_92bfc3;
    }

    // subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample -- halified (2 hals) exported (id=0x20802087) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__ = &subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__ = &subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_92bfc3;
    }

    // subdeviceCtrlCmdPerfGetPowerstate -- halified (2 hals) exported (id=0x2080205a) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfGetPowerstate__ = &subdeviceCtrlCmdPerfGetPowerstate_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfGetPowerstate__ = &subdeviceCtrlCmdPerfGetPowerstate_92bfc3;
    }

    // subdeviceCtrlCmdPerfNotifyVideoevent -- halified (2 hals) exported (id=0x2080205d) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdPerfNotifyVideoevent__ = &subdeviceCtrlCmdPerfNotifyVideoevent_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdPerfNotifyVideoevent__ = &subdeviceCtrlCmdPerfNotifyVideoevent_92bfc3;
    }

    // subdeviceCtrlCmdKPerfBoost -- exported (id=0x2080200a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
    pThis->__subdeviceCtrlCmdKPerfBoost__ = &subdeviceCtrlCmdKPerfBoost_IMPL;
#endif

    // subdeviceCtrlCmdFbGetFBRegionInfo -- exported (id=0x20801320)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__subdeviceCtrlCmdFbGetFBRegionInfo__ = &subdeviceCtrlCmdFbGetFBRegionInfo_IMPL;
#endif

    // subdeviceCtrlCmdFbGetBar1Offset -- exported (id=0x20801310)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbGetBar1Offset__ = &subdeviceCtrlCmdFbGetBar1Offset_IMPL;
#endif

    // subdeviceCtrlCmdFbIsKind -- exported (id=0x20801313)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__subdeviceCtrlCmdFbIsKind__ = &subdeviceCtrlCmdFbIsKind_IMPL;
#endif

    // subdeviceCtrlCmdFbGetMemAlignment -- exported (id=0x20801342)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdFbGetMemAlignment__ = &subdeviceCtrlCmdFbGetMemAlignment_IMPL;
#endif

    // subdeviceCtrlCmdFbGetHeapReservationSize -- exported (id=0x20800a5b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdFbGetHeapReservationSize__ = &subdeviceCtrlCmdFbGetHeapReservationSize_IMPL;
#endif

    // subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb -- exported (id=0x20800ac5)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb__ = &subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_IMPL;
#endif

    // subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout -- exported (id=0x20801352)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
    pThis->__subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout__ = &subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL;
#endif

    // subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp -- halified (singleton optimized) exported (id=0x20800afa) body
    pThis->__subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp__ = &subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_46f6a7;

    // subdeviceCtrlCmdFbGetInfoV2 -- exported (id=0x20801303)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdFbGetInfoV2__ = &subdeviceCtrlCmdFbGetInfoV2_IMPL;
#endif

    // subdeviceCtrlCmdFbGetCarveoutAddressInfo -- exported (id=0x2080130b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbGetCarveoutAddressInfo__ = &subdeviceCtrlCmdFbGetCarveoutAddressInfo_IMPL;
#endif

    // subdeviceCtrlCmdFbGetCalibrationLockFailed -- exported (id=0x2080130c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFbGetCalibrationLockFailed__ = &subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL;
#endif

    // subdeviceCtrlCmdFbFlushGpuCache -- exported (id=0x2080130e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdFbFlushGpuCache__ = &subdeviceCtrlCmdFbFlushGpuCache_IMPL;
#endif

    // subdeviceCtrlCmdFbSetGpuCacheAllocPolicy -- exported (id=0x2080130f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbSetGpuCacheAllocPolicy__ = &subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_IMPL;
#endif

    // subdeviceCtrlCmdFbGetGpuCacheAllocPolicy -- exported (id=0x20801312)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbGetGpuCacheAllocPolicy__ = &subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_IMPL;
#endif

    // subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2 -- exported (id=0x20801318)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2__ = &subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_IMPL;
#endif

    // subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2 -- exported (id=0x20801319)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2__ = &subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_IMPL;
#endif

    // subdeviceCtrlCmdFbGetGpuCacheInfo -- exported (id=0x20801315)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
    pThis->__subdeviceCtrlCmdFbGetGpuCacheInfo__ = &subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL;
#endif

    // subdeviceCtrlCmdFbGetCliManagedOfflinedPages -- exported (id=0x2080133c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__ = &subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL;
#endif

    // subdeviceCtrlCmdFbGetOfflinedPages -- halified (2 hals) exported (id=0x20801322) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFbGetOfflinedPages__ = &subdeviceCtrlCmdFbGetOfflinedPages_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFbGetOfflinedPages__ = &subdeviceCtrlCmdFbGetOfflinedPages_92bfc3;
    }

    // subdeviceCtrlCmdFbSetupVprRegion -- exported (id=0x2080133b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdFbSetupVprRegion__ = &subdeviceCtrlCmdFbSetupVprRegion_IMPL;
#endif

    // subdeviceCtrlCmdFbGetLTCInfoForFBP -- halified (2 hals) exported (id=0x20801328) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFbGetLTCInfoForFBP__ = &subdeviceCtrlCmdFbGetLTCInfoForFBP_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFbGetLTCInfoForFBP__ = &subdeviceCtrlCmdFbGetLTCInfoForFBP_92bfc3;
    }

    // subdeviceCtrlCmdFbGetCompBitCopyConstructInfo -- exported (id=0x2080133d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__ = &subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL;
#endif

    // subdeviceCtrlCmdFbPatchPbrForMining -- exported (id=0x20801341)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbPatchPbrForMining__ = &subdeviceCtrlCmdFbPatchPbrForMining_IMPL;
#endif

    // subdeviceCtrlCmdFbGetRemappedRows -- exported (id=0x20801344)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFbGetRemappedRows__ = &subdeviceCtrlCmdFbGetRemappedRows_IMPL;
#endif

    // subdeviceCtrlCmdFbGetFsInfo -- exported (id=0x20801346)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdFbGetFsInfo__ = &subdeviceCtrlCmdFbGetFsInfo_IMPL;
#endif

    // subdeviceCtrlCmdFbGetRowRemapperHistogram -- exported (id=0x20801347)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFbGetRowRemapperHistogram__ = &subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL;
#endif

    // subdeviceCtrlCmdFbGetDynamicOfflinedPages -- halified (2 hals) exported (id=0x20801348) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__ = &subdeviceCtrlCmdFbGetDynamicOfflinedPages_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__ = &subdeviceCtrlCmdFbGetDynamicOfflinedPages_92bfc3;
    }

    // subdeviceCtrlCmdFbUpdateNumaStatus -- exported (id=0x20801350)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdFbUpdateNumaStatus__ = &subdeviceCtrlCmdFbUpdateNumaStatus_IMPL;
#endif

    // subdeviceCtrlCmdFbGetNumaInfo -- exported (id=0x20801351)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdFbGetNumaInfo__ = &subdeviceCtrlCmdFbGetNumaInfo_IMPL;
#endif

    // subdeviceCtrlCmdMemSysGetStaticConfig -- halified (2 hals) exported (id=0x20800a1c) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysGetStaticConfig__ = &subdeviceCtrlCmdMemSysGetStaticConfig_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysGetStaticConfig__ = &subdeviceCtrlCmdMemSysGetStaticConfig_92bfc3;
    }

    // subdeviceCtrlCmdMemSysSetPartitionableMem -- exported (id=0x20800a51)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdMemSysSetPartitionableMem__ = &subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL;
#endif

    // subdeviceCtrlCmdKMemSysGetMIGMemoryConfig -- inline exported (id=0x20800a67) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__ = &subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e;
#endif

    // subdeviceCtrlCmdFbSetZbcReferenced -- exported (id=0x20800a69)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFbSetZbcReferenced__ = &subdeviceCtrlCmdFbSetZbcReferenced_IMPL;
#endif

    // subdeviceCtrlCmdMemSysL2InvalidateEvict -- exported (id=0x20800a6c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdMemSysL2InvalidateEvict__ = &subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL;
#endif

    // subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches -- exported (id=0x20800a6d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__ = &subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL;
#endif

    // subdeviceCtrlCmdMemSysDisableNvlinkPeers -- exported (id=0x20800a6e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdMemSysDisableNvlinkPeers__ = &subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL;
#endif

    // subdeviceCtrlCmdMemSysProgramRawCompressionMode -- exported (id=0x20800a6f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdMemSysProgramRawCompressionMode__ = &subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL;
#endif

    // subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable -- halified (2 hals) exported (id=0x20800a6b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__ = &subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__ = &subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_92bfc3;
    }

    // subdeviceCtrlCmdFbGetCtagsForCbcEviction -- exported (id=0x20801338)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbGetCtagsForCbcEviction__ = &subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL;
#endif

    // subdeviceCtrlCmdFbCBCOp -- exported (id=0x20801337)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbCBCOp__ = &subdeviceCtrlCmdFbCBCOp_IMPL;
#endif

    // subdeviceCtrlCmdFbSetRrd -- exported (id=0x2080133e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbSetRrd__ = &subdeviceCtrlCmdFbSetRrd_IMPL;
#endif

    // subdeviceCtrlCmdFbSetReadLimit -- exported (id=0x2080133f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbSetReadLimit__ = &subdeviceCtrlCmdFbSetReadLimit_IMPL;
#endif

    // subdeviceCtrlCmdFbSetWriteLimit -- exported (id=0x20801340)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFbSetWriteLimit__ = &subdeviceCtrlCmdFbSetWriteLimit_IMPL;
#endif

    // subdeviceCtrlCmdGmmuCommitTlbInvalidate -- exported (id=0x20801353)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
    pThis->__subdeviceCtrlCmdGmmuCommitTlbInvalidate__ = &subdeviceCtrlCmdGmmuCommitTlbInvalidate_IMPL;
#endif

    // subdeviceCtrlCmdSetGpfifo -- exported (id=0x20801102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdSetGpfifo__ = &subdeviceCtrlCmdSetGpfifo_IMPL;
#endif

    // subdeviceCtrlCmdSetOperationalProperties -- exported (id=0x20801104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdSetOperationalProperties__ = &subdeviceCtrlCmdSetOperationalProperties_IMPL;
#endif

    // subdeviceCtrlCmdFifoBindEngines -- exported (id=0x20801103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFifoBindEngines__ = &subdeviceCtrlCmdFifoBindEngines_IMPL;
#endif

    // subdeviceCtrlCmdGetPhysicalChannelCount -- exported (id=0x20801108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGetPhysicalChannelCount__ = &subdeviceCtrlCmdGetPhysicalChannelCount_IMPL;
#endif

    // subdeviceCtrlCmdFifoGetInfo -- exported (id=0x20801109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20008u)
    pThis->__subdeviceCtrlCmdFifoGetInfo__ = &subdeviceCtrlCmdFifoGetInfo_IMPL;
#endif

    // subdeviceCtrlCmdFifoDisableChannels -- exported (id=0x2080110b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__subdeviceCtrlCmdFifoDisableChannels__ = &subdeviceCtrlCmdFifoDisableChannels_IMPL;
#endif

    // subdeviceCtrlCmdFifoDisableChannelsForKeyRotation -- halified (2 hals) exported (id=0x2080111a) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_IMPL;
    }

    // subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2 -- halified (2 hals) exported (id=0x2080111b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__ = &subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_IMPL;
    }

    // subdeviceCtrlCmdFifoDisableUsermodeChannels -- exported (id=0x20801117)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdFifoDisableUsermodeChannels__ = &subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL;
#endif

    // subdeviceCtrlCmdFifoGetChannelMemInfo -- exported (id=0x2080110c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFifoGetChannelMemInfo__ = &subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL;
#endif

    // subdeviceCtrlCmdFifoGetUserdLocation -- exported (id=0x2080110d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFifoGetUserdLocation__ = &subdeviceCtrlCmdFifoGetUserdLocation_IMPL;
#endif

    // subdeviceCtrlCmdFifoObjschedSwGetLog -- exported (id=0x2080110e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFifoObjschedSwGetLog__ = &subdeviceCtrlCmdFifoObjschedSwGetLog_IMPL;
#endif

    // subdeviceCtrlCmdFifoObjschedGetState -- exported (id=0x20801120)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFifoObjschedGetState__ = &subdeviceCtrlCmdFifoObjschedGetState_IMPL;
#endif

    // subdeviceCtrlCmdFifoObjschedSetState -- exported (id=0x20801121)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFifoObjschedSetState__ = &subdeviceCtrlCmdFifoObjschedSetState_IMPL;
#endif

    // subdeviceCtrlCmdFifoObjschedGetCaps -- halified (2 hals) exported (id=0x20801122) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoObjschedGetCaps__ = &subdeviceCtrlCmdFifoObjschedGetCaps_ac1694;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoObjschedGetCaps__ = &subdeviceCtrlCmdFifoObjschedGetCaps_5baef9;
    }

    // subdeviceCtrlCmdFifoGetDeviceInfoTable -- halified (2 hals) exported (id=0x20801112) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoGetDeviceInfoTable__ = &subdeviceCtrlCmdFifoGetDeviceInfoTable_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoGetDeviceInfoTable__ = &subdeviceCtrlCmdFifoGetDeviceInfoTable_92bfc3;
    }

    // subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb -- exported (id=0x20801118)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb__ = &subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_IMPL;
#endif

    // subdeviceCtrlCmdFifoClearFaultedBit -- exported (id=0x20801113)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
    pThis->__subdeviceCtrlCmdFifoClearFaultedBit__ = &subdeviceCtrlCmdFifoClearFaultedBit_IMPL;
#endif

    // subdeviceCtrlCmdFifoRunlistSetSchedPolicy -- exported (id=0x20801115)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x68u)
    pThis->__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__ = &subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL;
#endif

    // subdeviceCtrlCmdFifoUpdateChannelInfo -- halified (2 hals) exported (id=0x20801116) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdFifoUpdateChannelInfo__ = &subdeviceCtrlCmdFifoUpdateChannelInfo_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdFifoUpdateChannelInfo__ = &subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL;
    }

    // subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers -- exported (id=0x20800a53)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__ = &subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL;
#endif

    // subdeviceCtrlCmdInternalFifoGetNumChannels -- exported (id=0x20800a61)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
    pThis->__subdeviceCtrlCmdInternalFifoGetNumChannels__ = &subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL;
#endif

    // subdeviceCtrlCmdFifoGetAllocatedChannels -- exported (id=0x20801119)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFifoGetAllocatedChannels__ = &subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL;
#endif

    // subdeviceCtrlCmdInternalFifoGetNumSecureChannels -- exported (id=0x20800ad8)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalFifoGetNumSecureChannels__ = &subdeviceCtrlCmdInternalFifoGetNumSecureChannels_IMPL;
#endif

    // subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling -- exported (id=0x20800ac3)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling__ = &subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetInfo -- exported (id=0x20801201)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrGetInfo__ = &subdeviceCtrlCmdKGrGetInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetInfoV2 -- exported (id=0x20801228)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrGetInfoV2__ = &subdeviceCtrlCmdKGrGetInfoV2_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetCapsV2 -- exported (id=0x20801227)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
    pThis->__subdeviceCtrlCmdKGrGetCapsV2__ = &subdeviceCtrlCmdKGrGetCapsV2_IMPL;
#endif
} // End __nvoc_init_funcTable_Subdevice_1 with approximately 263 basic block(s).

// Vtable initialization 2/3
static void __nvoc_init_funcTable_Subdevice_2(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // subdeviceCtrlCmdKGrGetCtxswModes -- exported (id=0x20801235)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrGetCtxswModes__ = &subdeviceCtrlCmdKGrGetCtxswModes_IMPL;
#endif

    // subdeviceCtrlCmdKGrCtxswZcullMode -- exported (id=0x20801205)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdKGrCtxswZcullMode__ = &subdeviceCtrlCmdKGrCtxswZcullMode_IMPL;
#endif

    // subdeviceCtrlCmdKGrCtxswZcullBind -- exported (id=0x20801208)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80348u)
    pThis->__subdeviceCtrlCmdKGrCtxswZcullBind__ = &subdeviceCtrlCmdKGrCtxswZcullBind_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetZcullInfo -- exported (id=0x20801206)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__subdeviceCtrlCmdKGrGetZcullInfo__ = &subdeviceCtrlCmdKGrGetZcullInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrCtxswPmMode -- halified (2 hals) exported (id=0x20801207) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdKGrCtxswPmMode__ = &subdeviceCtrlCmdKGrCtxswPmMode_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdKGrCtxswPmMode__ = &subdeviceCtrlCmdKGrCtxswPmMode_IMPL;
    }

    // subdeviceCtrlCmdKGrCtxswPmBind -- exported (id=0x20801209)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrCtxswPmBind__ = &subdeviceCtrlCmdKGrCtxswPmBind_IMPL;
#endif

    // subdeviceCtrlCmdKGrCtxswSetupBind -- exported (id=0x2080123a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrCtxswSetupBind__ = &subdeviceCtrlCmdKGrCtxswSetupBind_IMPL;
#endif

    // subdeviceCtrlCmdKGrSetGpcTileMap -- exported (id=0x2080120a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrSetGpcTileMap__ = &subdeviceCtrlCmdKGrSetGpcTileMap_IMPL;
#endif

    // subdeviceCtrlCmdKGrCtxswSmpcMode -- exported (id=0x2080120e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrCtxswSmpcMode__ = &subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL;
#endif

    // subdeviceCtrlCmdKGrPcSamplingMode -- exported (id=0x20801212)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdKGrPcSamplingMode__ = &subdeviceCtrlCmdKGrPcSamplingMode_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetSmToGpcTpcMappings -- exported (id=0x2080120f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__ = &subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetGlobalSmOrder -- exported (id=0x2080121b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrGetGlobalSmOrder__ = &subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL;
#endif

    // subdeviceCtrlCmdKGrSetCtxswPreemptionMode -- exported (id=0x20801210)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x348u)
    pThis->__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__ = &subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL;
#endif

    // subdeviceCtrlCmdKGrCtxswPreemptionBind -- exported (id=0x20801211)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdKGrCtxswPreemptionBind__ = &subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetROPInfo -- exported (id=0x20801213)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetROPInfo__ = &subdeviceCtrlCmdKGrGetROPInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetCtxswStats -- exported (id=0x20801215)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrGetCtxswStats__ = &subdeviceCtrlCmdKGrGetCtxswStats_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetCtxBufferSize -- exported (id=0x20801218)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdKGrGetCtxBufferSize__ = &subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetCtxBufferInfo -- exported (id=0x20801219)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
    pThis->__subdeviceCtrlCmdKGrGetCtxBufferInfo__ = &subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetCtxBufferPtes -- exported (id=0x20800a28)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
    pThis->__subdeviceCtrlCmdKGrGetCtxBufferPtes__ = &subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetCurrentResidentChannel -- exported (id=0x2080121c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrGetCurrentResidentChannel__ = &subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetVatAlarmData -- exported (id=0x2080121d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrGetVatAlarmData__ = &subdeviceCtrlCmdKGrGetVatAlarmData_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetAttributeBufferSize -- exported (id=0x2080121e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetAttributeBufferSize__ = &subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL;
#endif

    // subdeviceCtrlCmdKGrGfxPoolQuerySize -- exported (id=0x2080121f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolQuerySize__ = &subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL;
#endif

    // subdeviceCtrlCmdKGrGfxPoolInitialize -- exported (id=0x20801220)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolInitialize__ = &subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL;
#endif

    // subdeviceCtrlCmdKGrGfxPoolAddSlots -- exported (id=0x20801221)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolAddSlots__ = &subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL;
#endif

    // subdeviceCtrlCmdKGrGfxPoolRemoveSlots -- exported (id=0x20801222)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__ = &subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetPpcMask -- exported (id=0x20801233)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetPpcMask__ = &subdeviceCtrlCmdKGrGetPpcMask_IMPL;
#endif

    // subdeviceCtrlCmdKGrSetTpcPartitionMode -- exported (id=0x2080122c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrSetTpcPartitionMode__ = &subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetSmIssueRateModifier -- exported (id=0x20801230)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetSmIssueRateModifier__ = &subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL;
#endif

    // subdeviceCtrlCmdKGrFecsBindEvtbufForUid -- exported (id=0x20801231)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__ = &subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL;
#endif

    // subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2 -- exported (id=0x20801238)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
    pThis->__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__ = &subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetPhysGpcMask -- exported (id=0x20801232)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14u)
    pThis->__subdeviceCtrlCmdKGrGetPhysGpcMask__ = &subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetGpcMask -- exported (id=0x2080122a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrGetGpcMask__ = &subdeviceCtrlCmdKGrGetGpcMask_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetTpcMask -- exported (id=0x2080122b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdKGrGetTpcMask__ = &subdeviceCtrlCmdKGrGetTpcMask_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetEngineContextProperties -- exported (id=0x2080122d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetEngineContextProperties__ = &subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetNumTpcsForGpc -- exported (id=0x20801234)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetNumTpcsForGpc__ = &subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetGpcTileMap -- exported (id=0x20801236)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdKGrGetGpcTileMap__ = &subdeviceCtrlCmdKGrGetGpcTileMap_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetZcullMask -- exported (id=0x20801237)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdKGrGetZcullMask__ = &subdeviceCtrlCmdKGrGetZcullMask_IMPL;
#endif

    // subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo -- exported (id=0x20801239)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo__ = &subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetInfo -- exported (id=0x20800a2a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetCaps -- exported (id=0x20800a1f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetCaps__ = &subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder -- exported (id=0x20800a22)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__ = &subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks -- exported (id=0x20800a26)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__ = &subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetPpcMasks -- exported (id=0x20800a30)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__ = &subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetZcullInfo -- exported (id=0x20800a2c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetRopInfo -- exported (id=0x20800a2e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo -- exported (id=0x20800a32)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c1c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier -- exported (id=0x20800a34)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__ = &subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize -- exported (id=0x20800a3d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__ = &subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines -- exported (id=0x20800a3f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__ = &subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalStaticGetPdbProperties -- exported (id=0x20800a48)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c0c0u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__ = &subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL;
#endif

    // subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable -- exported (id=0x20800a37)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable__ = &subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL;
#endif

    // subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable -- exported (id=0x20800a38)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable__ = &subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL;
#endif

    // subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset -- exported (id=0x20800a39)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset__ = &subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL;
#endif

    // subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset -- exported (id=0x20800a3b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset__ = &subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL;
#endif

    // subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset -- exported (id=0x20800a3a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset__ = &subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL;
#endif

    // subdeviceCtrlCmdGrStaticGetFecsTraceDefines -- exported (id=0x20800a3e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10040u)
    pThis->__subdeviceCtrlCmdGrStaticGetFecsTraceDefines__ = &subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL;
#endif

    // subdeviceCtrlCmdKGrInternalInitBug4208224War -- exported (id=0x20800a46)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
    pThis->__subdeviceCtrlCmdKGrInternalInitBug4208224War__ = &subdeviceCtrlCmdKGrInternalInitBug4208224War_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetCachedInfo -- exported (id=0x20800182)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__subdeviceCtrlCmdGpuGetCachedInfo__ = &subdeviceCtrlCmdGpuGetCachedInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetInfoV2 -- exported (id=0x20800102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x20118u)
    pThis->__subdeviceCtrlCmdGpuGetInfoV2__ = &subdeviceCtrlCmdGpuGetInfoV2_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetIpVersion -- exported (id=0x2080014d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetIpVersion__ = &subdeviceCtrlCmdGpuGetIpVersion_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo -- exported (id=0x2080015a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__ = &subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu -- exported (id=0x2080015b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__ = &subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetOptimusInfo -- exported (id=0x2080014c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuSetOptimusInfo__ = &subdeviceCtrlCmdGpuSetOptimusInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetNameString -- exported (id=0x20800110)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010au)
    pThis->__subdeviceCtrlCmdGpuGetNameString__ = &subdeviceCtrlCmdGpuGetNameString_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetShortNameString -- exported (id=0x20800111)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50au)
    pThis->__subdeviceCtrlCmdGpuGetShortNameString__ = &subdeviceCtrlCmdGpuGetShortNameString_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEncoderCapacity -- exported (id=0x2080016c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetEncoderCapacity__ = &subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetNvencSwSessionStats -- exported (id=0x2080016d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetNvencSwSessionStats__ = &subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetNvencSwSessionInfo -- exported (id=0x2080016e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__ = &subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2 -- exported (id=0x208001af)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2__ = &subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetNvfbcSwSessionStats -- exported (id=0x2080017b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__ = &subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo -- exported (id=0x2080017c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__ = &subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetFabricAddr -- exported (id=0x2080016f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuSetFabricAddr__ = &subdeviceCtrlCmdGpuSetFabricAddr_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetEgmGpaFabricAddr -- exported (id=0x20800199)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuSetEgmGpaFabricAddr__ = &subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetPower -- exported (id=0x20800112)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuSetPower__ = &subdeviceCtrlCmdGpuSetPower_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetSdm -- exported (id=0x20800118)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__subdeviceCtrlCmdGpuGetSdm__ = &subdeviceCtrlCmdGpuGetSdm_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetSdm -- exported (id=0x20800120)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
    pThis->__subdeviceCtrlCmdGpuSetSdm__ = &subdeviceCtrlCmdGpuSetSdm_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetSimulationInfo -- exported (id=0x20800119)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50bu)
    pThis->__subdeviceCtrlCmdGpuGetSimulationInfo__ = &subdeviceCtrlCmdGpuGetSimulationInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEngines -- exported (id=0x20800123)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__subdeviceCtrlCmdGpuGetEngines__ = &subdeviceCtrlCmdGpuGetEngines_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEnginesV2 -- exported (id=0x20800170)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__subdeviceCtrlCmdGpuGetEnginesV2__ = &subdeviceCtrlCmdGpuGetEnginesV2_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEngineClasslist -- exported (id=0x20800124)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__subdeviceCtrlCmdGpuGetEngineClasslist__ = &subdeviceCtrlCmdGpuGetEngineClasslist_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEnginePartnerList -- exported (id=0x20800147)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetEnginePartnerList__ = &subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetFermiGpcInfo -- exported (id=0x20800137)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetFermiGpcInfo__ = &subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetFermiTpcInfo -- exported (id=0x20800138)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetFermiTpcInfo__ = &subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetFermiZcullInfo -- exported (id=0x20800139)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetFermiZcullInfo__ = &subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetPesInfo -- exported (id=0x20800168)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetPesInfo__ = &subdeviceCtrlCmdGpuGetPesInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuExecRegOps -- exported (id=0x20800122)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuExecRegOps__ = &subdeviceCtrlCmdGpuExecRegOps_IMPL;
#endif

    // subdeviceCtrlCmdGpuMigratableOps -- exported (id=0x208001a6)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuMigratableOps__ = &subdeviceCtrlCmdGpuMigratableOps_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryMode -- exported (id=0x20800128)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuQueryMode__ = &subdeviceCtrlCmdGpuQueryMode_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetInforomImageVersion -- exported (id=0x20800156)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
    pThis->__subdeviceCtrlCmdGpuGetInforomImageVersion__ = &subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetInforomObjectVersion -- exported (id=0x2080014b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetInforomObjectVersion__ = &subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryInforomEccSupport -- exported (id=0x20800157)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuQueryInforomEccSupport__ = &subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryEccStatus -- halified (2 hals) exported (id=0x2080012f) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccStatus__ = &subdeviceCtrlCmdGpuQueryEccStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccStatus__ = &subdeviceCtrlCmdGpuQueryEccStatus_92bfc3;
    }

    // subdeviceCtrlCmdGpuGetChipDetails -- exported (id=0x208001a4)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40448u)
    pThis->__subdeviceCtrlCmdGpuGetChipDetails__ = &subdeviceCtrlCmdGpuGetChipDetails_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetOEMBoardInfo -- exported (id=0x2080013f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
    pThis->__subdeviceCtrlCmdGpuGetOEMBoardInfo__ = &subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetOEMInfo -- exported (id=0x20800169)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
    pThis->__subdeviceCtrlCmdGpuGetOEMInfo__ = &subdeviceCtrlCmdGpuGetOEMInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuHandleGpuSR -- exported (id=0x20800167)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
    pThis->__subdeviceCtrlCmdGpuHandleGpuSR__ = &subdeviceCtrlCmdGpuHandleGpuSR_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetComputeModeRules -- exported (id=0x20800130)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x114u)
    pThis->__subdeviceCtrlCmdGpuSetComputeModeRules__ = &subdeviceCtrlCmdGpuSetComputeModeRules_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryComputeModeRules -- exported (id=0x20800131)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__subdeviceCtrlCmdGpuQueryComputeModeRules__ = &subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL;
#endif

    // subdeviceCtrlCmdGpuAcquireComputeModeReservation -- exported (id=0x20800145)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuAcquireComputeModeReservation__ = &subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL;
#endif

    // subdeviceCtrlCmdGpuReleaseComputeModeReservation -- exported (id=0x20800146)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuReleaseComputeModeReservation__ = &subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL;
#endif

    // subdeviceCtrlCmdGpuInitializeCtx -- exported (id=0x2080012d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14244u)
    pThis->__subdeviceCtrlCmdGpuInitializeCtx__ = &subdeviceCtrlCmdGpuInitializeCtx_IMPL;
#endif

    // subdeviceCtrlCmdGpuPromoteCtx -- exported (id=0x2080012b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
    pThis->__subdeviceCtrlCmdGpuPromoteCtx__ = &subdeviceCtrlCmdGpuPromoteCtx_IMPL;
#endif

    // subdeviceCtrlCmdGpuEvictCtx -- exported (id=0x2080012c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c240u)
    pThis->__subdeviceCtrlCmdGpuEvictCtx__ = &subdeviceCtrlCmdGpuEvictCtx_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetId -- exported (id=0x20800142)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
    pThis->__subdeviceCtrlCmdGpuGetId__ = &subdeviceCtrlCmdGpuGetId_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetGidInfo -- exported (id=0x2080014a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetGidInfo__ = &subdeviceCtrlCmdGpuGetGidInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryIllumSupport -- halified (2 hals) exported (id=0x20800153) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryIllumSupport__ = &subdeviceCtrlCmdGpuQueryIllumSupport_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryIllumSupport__ = &subdeviceCtrlCmdGpuQueryIllumSupport_5baef9;
    }

    // subdeviceCtrlCmdGpuGetIllum -- exported (id=0x20800154)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetIllum__ = &subdeviceCtrlCmdGpuGetIllum_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetIllum -- exported (id=0x20800155)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuSetIllum__ = &subdeviceCtrlCmdGpuSetIllum_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryScrubberStatus -- halified (2 hals) exported (id=0x2080015f) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryScrubberStatus__ = &subdeviceCtrlCmdGpuQueryScrubberStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryScrubberStatus__ = &subdeviceCtrlCmdGpuQueryScrubberStatus_92bfc3;
    }

    // subdeviceCtrlCmdGpuGetVprCaps -- exported (id=0x20800160)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetVprCaps__ = &subdeviceCtrlCmdGpuGetVprCaps_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetVprInfo -- exported (id=0x2080016b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetVprInfo__ = &subdeviceCtrlCmdGpuGetVprInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetPids -- exported (id=0x2080018d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetPids__ = &subdeviceCtrlCmdGpuGetPids_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetPidInfo -- exported (id=0x2080018e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetPidInfo__ = &subdeviceCtrlCmdGpuGetPidInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryFunctionStatus -- exported (id=0x20800173)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuQueryFunctionStatus__ = &subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL;
#endif

    // subdeviceCtrlCmdGpuReportNonReplayableFault -- exported (id=0x20800177)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40040u)
    pThis->__subdeviceCtrlCmdGpuReportNonReplayableFault__ = &subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEngineFaultInfo -- exported (id=0x20800125)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__subdeviceCtrlCmdGpuGetEngineFaultInfo__ = &subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEngineRunlistPriBase -- exported (id=0x20800179)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__ = &subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetHwEngineId -- exported (id=0x2080017a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetHwEngineId__ = &subdeviceCtrlCmdGpuGetHwEngineId_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetVmmuSegmentSize -- exported (id=0x2080017e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x448u)
    pThis->__subdeviceCtrlCmdGpuGetVmmuSegmentSize__ = &subdeviceCtrlCmdGpuGetVmmuSegmentSize_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetMaxSupportedPageSize -- exported (id=0x20800188)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__ = &subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL;
#endif

    // subdeviceCtrlCmdGpuHandleVfPriFault -- exported (id=0x20800192)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdGpuHandleVfPriFault__ = &subdeviceCtrlCmdGpuHandleVfPriFault_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetComputePolicyConfig -- exported (id=0x20800194)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
    pThis->__subdeviceCtrlCmdGpuSetComputePolicyConfig__ = &subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetComputePolicyConfig -- exported (id=0x20800195)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__subdeviceCtrlCmdGpuGetComputePolicyConfig__ = &subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL;
#endif

    // subdeviceCtrlCmdValidateMemMapRequest -- exported (id=0x20800198)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
    pThis->__subdeviceCtrlCmdValidateMemMapRequest__ = &subdeviceCtrlCmdValidateMemMapRequest_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetGfid -- exported (id=0x20800196)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetGfid__ = &subdeviceCtrlCmdGpuGetGfid_IMPL;
#endif

    // subdeviceCtrlCmdUpdateGfidP2pCapability -- exported (id=0x20800197)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdUpdateGfidP2pCapability__ = &subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetEngineLoadTimes -- exported (id=0x2080019b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
    pThis->__subdeviceCtrlCmdGpuGetEngineLoadTimes__ = &subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL;
#endif

    // subdeviceCtrlCmdGetP2pCaps -- exported (id=0x208001a0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__subdeviceCtrlCmdGetP2pCaps__ = &subdeviceCtrlCmdGetP2pCaps_IMPL;
#endif

    // subdeviceCtrlCmdGetGpuFabricProbeInfo -- exported (id=0x208001a3)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGetGpuFabricProbeInfo__ = &subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetResetStatus -- exported (id=0x208001ab)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetResetStatus__ = &subdeviceCtrlCmdGpuGetResetStatus_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetDrainAndResetStatus -- exported (id=0x208001ae)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetDrainAndResetStatus__ = &subdeviceCtrlCmdGpuGetDrainAndResetStatus_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetConstructedFalconInfo -- exported (id=0x208001b0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuGetConstructedFalconInfo__ = &subdeviceCtrlCmdGpuGetConstructedFalconInfo_IMPL;
#endif

    // subdeviceCtrlGpuGetFipsStatus -- exported (id=0x208001e4)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlGpuGetFipsStatus__ = &subdeviceCtrlGpuGetFipsStatus_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetVfCaps -- exported (id=0x208001b1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__subdeviceCtrlCmdGpuGetVfCaps__ = &subdeviceCtrlCmdGpuGetVfCaps_IMPL;
#endif

    // subdeviceCtrlCmdEventSetTrigger -- exported (id=0x20800302)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdEventSetTrigger__ = &subdeviceCtrlCmdEventSetTrigger_IMPL;
#endif

    // subdeviceCtrlCmdEventSetTriggerFifo -- exported (id=0x20800308)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__subdeviceCtrlCmdEventSetTriggerFifo__ = &subdeviceCtrlCmdEventSetTriggerFifo_IMPL;
#endif

    // subdeviceCtrlCmdEventSetNotification -- exported (id=0x20800301)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdEventSetNotification__ = &subdeviceCtrlCmdEventSetNotification_IMPL;
#endif

    // subdeviceCtrlCmdEventSetMemoryNotifies -- exported (id=0x20800303)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdEventSetMemoryNotifies__ = &subdeviceCtrlCmdEventSetMemoryNotifies_IMPL;
#endif

    // subdeviceCtrlCmdEventSetSemaphoreMemory -- exported (id=0x20800304)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdEventSetSemaphoreMemory__ = &subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL;
#endif

    // subdeviceCtrlCmdEventSetSemaMemValidation -- exported (id=0x20800306)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdEventSetSemaMemValidation__ = &subdeviceCtrlCmdEventSetSemaMemValidation_IMPL;
#endif

    // subdeviceCtrlCmdEventVideoBindEvtbuf -- exported (id=0x20800309)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdEventVideoBindEvtbuf__ = &subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL;
#endif

    // subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf -- exported (id=0x2080030a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf__ = &subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL;
#endif

    // subdeviceCtrlCmdTimerCancel -- exported (id=0x20800402)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdTimerCancel__ = &subdeviceCtrlCmdTimerCancel_IMPL;
#endif

    // subdeviceCtrlCmdTimerSchedule -- exported (id=0x20800401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdTimerSchedule__ = &subdeviceCtrlCmdTimerSchedule_IMPL;
#endif

    // subdeviceCtrlCmdTimerGetTime -- exported (id=0x20800403)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdTimerGetTime__ = &subdeviceCtrlCmdTimerGetTime_IMPL;
#endif

    // subdeviceCtrlCmdTimerGetRegisterOffset -- exported (id=0x20800404)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__subdeviceCtrlCmdTimerGetRegisterOffset__ = &subdeviceCtrlCmdTimerGetRegisterOffset_IMPL;
#endif

    // subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo -- exported (id=0x20800406)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__ = &subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL;
#endif

    // subdeviceCtrlCmdTimerSetGrTickFreq -- exported (id=0x20800407)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
    pThis->__subdeviceCtrlCmdTimerSetGrTickFreq__ = &subdeviceCtrlCmdTimerSetGrTickFreq_IMPL;
#endif

    // subdeviceCtrlCmdRcReadVirtualMem -- exported (id=0x20802204)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdRcReadVirtualMem__ = &subdeviceCtrlCmdRcReadVirtualMem_IMPL;
#endif

    // subdeviceCtrlCmdRcGetErrorCount -- exported (id=0x20802205)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdRcGetErrorCount__ = &subdeviceCtrlCmdRcGetErrorCount_IMPL;
#endif

    // subdeviceCtrlCmdRcGetErrorV2 -- exported (id=0x20802213)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdRcGetErrorV2__ = &subdeviceCtrlCmdRcGetErrorV2_IMPL;
#endif

    // subdeviceCtrlCmdRcSetCleanErrorHistory -- exported (id=0x20802207)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdRcSetCleanErrorHistory__ = &subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL;
#endif

    // subdeviceCtrlCmdRcGetWatchdogInfo -- exported (id=0x20802209)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdRcGetWatchdogInfo__ = &subdeviceCtrlCmdRcGetWatchdogInfo_IMPL;
#endif

    // subdeviceCtrlCmdRcDisableWatchdog -- exported (id=0x2080220a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdRcDisableWatchdog__ = &subdeviceCtrlCmdRcDisableWatchdog_IMPL;
#endif

    // subdeviceCtrlCmdRcSoftDisableWatchdog -- exported (id=0x20802210)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdRcSoftDisableWatchdog__ = &subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL;
#endif

    // subdeviceCtrlCmdRcEnableWatchdog -- exported (id=0x2080220b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdRcEnableWatchdog__ = &subdeviceCtrlCmdRcEnableWatchdog_IMPL;
#endif

    // subdeviceCtrlCmdRcReleaseWatchdogRequests -- exported (id=0x2080220c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x18u)
    pThis->__subdeviceCtrlCmdRcReleaseWatchdogRequests__ = &subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL;
#endif

    // subdeviceCtrlCmdInternalRcWatchdogTimeout -- exported (id=0x20800a6a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalRcWatchdogTimeout__ = &subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL;
#endif

    // subdeviceCtrlCmdSetRcRecovery -- halified (2 hals) exported (id=0x2080220d) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdSetRcRecovery__ = &subdeviceCtrlCmdSetRcRecovery_56cd7a;
    }
    else
    {
        pThis->__subdeviceCtrlCmdSetRcRecovery__ = &subdeviceCtrlCmdSetRcRecovery_5baef9;
    }

    // subdeviceCtrlCmdGetRcRecovery -- halified (2 hals) exported (id=0x2080220e) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGetRcRecovery__ = &subdeviceCtrlCmdGetRcRecovery_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGetRcRecovery__ = &subdeviceCtrlCmdGetRcRecovery_5baef9;
    }

    // subdeviceCtrlCmdGetRcInfo -- exported (id=0x20802212)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdGetRcInfo__ = &subdeviceCtrlCmdGetRcInfo_IMPL;
#endif

    // subdeviceCtrlCmdSetRcInfo -- exported (id=0x20802211)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdSetRcInfo__ = &subdeviceCtrlCmdSetRcInfo_IMPL;
#endif

    // subdeviceCtrlCmdNvdGetDumpSize -- exported (id=0x20802401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdNvdGetDumpSize__ = &subdeviceCtrlCmdNvdGetDumpSize_IMPL;
#endif

    // subdeviceCtrlCmdNvdGetDump -- exported (id=0x20802402)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdNvdGetDump__ = &subdeviceCtrlCmdNvdGetDump_IMPL;
#endif

    // subdeviceCtrlCmdNvdGetNocatJournalRpt -- exported (id=0x20802409)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdNvdGetNocatJournalRpt__ = &subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL;
#endif

    // subdeviceCtrlCmdNvdSetNocatJournalData -- exported (id=0x2080240b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdNvdSetNocatJournalData__ = &subdeviceCtrlCmdNvdSetNocatJournalData_IMPL;
#endif

    // subdeviceCtrlCmdNvdInsertNocatJournalRecord -- exported (id=0x2080240c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdNvdInsertNocatJournalRecord__ = &subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL;
#endif

    // subdeviceCtrlCmdPmgrGetModuleInfo -- exported (id=0x20802609)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
    pThis->__subdeviceCtrlCmdPmgrGetModuleInfo__ = &subdeviceCtrlCmdPmgrGetModuleInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuProcessPostGc6ExitTasks -- exported (id=0x2080270c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGpuProcessPostGc6ExitTasks__ = &subdeviceCtrlCmdGpuProcessPostGc6ExitTasks_IMPL;
#endif

    // subdeviceCtrlCmdGc6Entry -- exported (id=0x2080270d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
    pThis->__subdeviceCtrlCmdGc6Entry__ = &subdeviceCtrlCmdGc6Entry_IMPL;
#endif

    // subdeviceCtrlCmdGc6Exit -- exported (id=0x2080270e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2u)
    pThis->__subdeviceCtrlCmdGc6Exit__ = &subdeviceCtrlCmdGc6Exit_IMPL;
#endif

    // subdeviceCtrlCmdLpwrDifrPrefetchResponse -- exported (id=0x20802802)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__subdeviceCtrlCmdLpwrDifrPrefetchResponse__ = &subdeviceCtrlCmdLpwrDifrPrefetchResponse_IMPL;
#endif

    // subdeviceCtrlCmdLpwrDifrCtrl -- exported (id=0x20802801)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdLpwrDifrCtrl__ = &subdeviceCtrlCmdLpwrDifrCtrl_IMPL;
#endif

    // subdeviceCtrlCmdCeGetCaps -- halified (2 hals) exported (id=0x20802a01)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCeGetCaps__ = &subdeviceCtrlCmdCeGetCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCeGetCaps__ = &subdeviceCtrlCmdCeGetCaps_IMPL;
    }

    // subdeviceCtrlCmdCeGetCePceMask -- exported (id=0x20802a02)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x349u)
    pThis->__subdeviceCtrlCmdCeGetCePceMask__ = &subdeviceCtrlCmdCeGetCePceMask_IMPL;
#endif

    // subdeviceCtrlCmdCeUpdatePceLceMappings -- exported (id=0x20802a05)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdCeUpdatePceLceMappings__ = &subdeviceCtrlCmdCeUpdatePceLceMappings_IMPL;
#endif

    // subdeviceCtrlCmdCeUpdatePceLceMappingsV2 -- exported (id=0x20802a0d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdCeUpdatePceLceMappingsV2__ = &subdeviceCtrlCmdCeUpdatePceLceMappingsV2_IMPL;
#endif

    // subdeviceCtrlCmdCeGetLceShimInfo -- exported (id=0x20802a0c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x145u)
    pThis->__subdeviceCtrlCmdCeGetLceShimInfo__ = &subdeviceCtrlCmdCeGetLceShimInfo_IMPL;
#endif

    // subdeviceCtrlCmdCeGetPceConfigForLceType -- exported (id=0x20802a0f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdCeGetPceConfigForLceType__ = &subdeviceCtrlCmdCeGetPceConfigForLceType_IMPL;
#endif

    // subdeviceCtrlCmdCeGetCapsV2 -- halified (2 hals) exported (id=0x20802a03)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCeGetCapsV2__ = &subdeviceCtrlCmdCeGetCapsV2_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCeGetCapsV2__ = &subdeviceCtrlCmdCeGetCapsV2_IMPL;
    }

    // subdeviceCtrlCmdCeGetAllCaps -- halified (2 hals) exported (id=0x20802a0a)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdCeGetAllCaps__ = &subdeviceCtrlCmdCeGetAllCaps_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdCeGetAllCaps__ = &subdeviceCtrlCmdCeGetAllCaps_IMPL;
    }

    // subdeviceCtrlCmdCeGetDecompLceMask -- exported (id=0x20802a11)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x154u)
    pThis->__subdeviceCtrlCmdCeGetDecompLceMask__ = &subdeviceCtrlCmdCeGetDecompLceMask_IMPL;
#endif

    // subdeviceCtrlCmdFlcnGetDmemUsage -- exported (id=0x20803101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFlcnGetDmemUsage__ = &subdeviceCtrlCmdFlcnGetDmemUsage_IMPL;
#endif

    // subdeviceCtrlCmdFlcnGetEngineArch -- exported (id=0x20803118)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFlcnGetEngineArch__ = &subdeviceCtrlCmdFlcnGetEngineArch_IMPL;
#endif

    // subdeviceCtrlCmdFlcnUstreamerQueueInfo -- exported (id=0x20803120)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFlcnUstreamerQueueInfo__ = &subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL;
#endif

    // subdeviceCtrlCmdFlcnUstreamerControlGet -- exported (id=0x20803122)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdFlcnUstreamerControlGet__ = &subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL;
#endif

    // subdeviceCtrlCmdFlcnUstreamerControlSet -- exported (id=0x20803123)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdFlcnUstreamerControlSet__ = &subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL;
#endif

    // subdeviceCtrlCmdFlcnGetCtxBufferInfo -- exported (id=0x20803124)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdFlcnGetCtxBufferInfo__ = &subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL;
#endif

    // subdeviceCtrlCmdFlcnGetCtxBufferSize -- exported (id=0x20803125)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFlcnGetCtxBufferSize__ = &subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL;
#endif

    // subdeviceCtrlCmdEccGetClientExposedCounters -- exported (id=0x20803400)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdEccGetClientExposedCounters__ = &subdeviceCtrlCmdEccGetClientExposedCounters_IMPL;
#endif

    // subdeviceCtrlCmdEccGetEciCounters -- exported (id=0x20803401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdEccGetEciCounters__ = &subdeviceCtrlCmdEccGetEciCounters_IMPL;
#endif

    // subdeviceCtrlCmdEccGetVolatileCounts -- exported (id=0x20803402)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdEccGetVolatileCounts__ = &subdeviceCtrlCmdEccGetVolatileCounts_IMPL;
#endif

    // subdeviceCtrlCmdGpuQueryEccConfiguration -- halified (2 hals) exported (id=0x20800133) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccConfiguration__ = &subdeviceCtrlCmdGpuQueryEccConfiguration_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuQueryEccConfiguration__ = &subdeviceCtrlCmdGpuQueryEccConfiguration_92bfc3;
    }

    // subdeviceCtrlCmdGpuSetEccConfiguration -- halified (2 hals) exported (id=0x20800134) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuSetEccConfiguration__ = &subdeviceCtrlCmdGpuSetEccConfiguration_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuSetEccConfiguration__ = &subdeviceCtrlCmdGpuSetEccConfiguration_92bfc3;
    }

    // subdeviceCtrlCmdGpuResetEccErrorStatus -- halified (2 hals) exported (id=0x20800136) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGpuResetEccErrorStatus__ = &subdeviceCtrlCmdGpuResetEccErrorStatus_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGpuResetEccErrorStatus__ = &subdeviceCtrlCmdGpuResetEccErrorStatus_92bfc3;
    }

    // subdeviceCtrlCmdFlaRange -- exported (id=0x20803501)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdFlaRange__ = &subdeviceCtrlCmdFlaRange_IMPL;
#endif

    // subdeviceCtrlCmdFlaSetupInstanceMemBlock -- exported (id=0x20803502)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10244u)
    pThis->__subdeviceCtrlCmdFlaSetupInstanceMemBlock__ = &subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL;
#endif

    // subdeviceCtrlCmdFlaGetRange -- exported (id=0x20803503)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
    pThis->__subdeviceCtrlCmdFlaGetRange__ = &subdeviceCtrlCmdFlaGetRange_IMPL;
#endif

    // subdeviceCtrlCmdFlaGetFabricMemStats -- exported (id=0x20803504)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__subdeviceCtrlCmdFlaGetFabricMemStats__ = &subdeviceCtrlCmdFlaGetFabricMemStats_IMPL;
#endif

    // subdeviceCtrlCmdGspGetFeatures -- halified (2 hals) exported (id=0x20803601) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdGspGetFeatures__ = &subdeviceCtrlCmdGspGetFeatures_KERNEL;
    }
    else
    {
        pThis->__subdeviceCtrlCmdGspGetFeatures__ = &subdeviceCtrlCmdGspGetFeatures_92bfc3;
    }

    // subdeviceCtrlCmdGspGetRmHeapStats -- exported (id=0x20803602)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__subdeviceCtrlCmdGspGetRmHeapStats__ = &subdeviceCtrlCmdGspGetRmHeapStats_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetActivePartitionIds -- exported (id=0x2080018b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetActivePartitionIds__ = &subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetPartitionCapacity -- exported (id=0x20800181)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__subdeviceCtrlCmdGpuGetPartitionCapacity__ = &subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL;
#endif

    // subdeviceCtrlCmdGpuDescribePartitions -- exported (id=0x20800185)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuDescribePartitions__ = &subdeviceCtrlCmdGpuDescribePartitions_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetPartitioningMode -- exported (id=0x20800183)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdGpuSetPartitioningMode__ = &subdeviceCtrlCmdGpuSetPartitioningMode_IMPL;
#endif

    // subdeviceCtrlCmdGrmgrGetGrFsInfo -- exported (id=0x20803801)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__subdeviceCtrlCmdGrmgrGetGrFsInfo__ = &subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL;
#endif

    // subdeviceCtrlCmdGpuSetPartitions -- exported (id=0x20800174)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuSetPartitions__ = &subdeviceCtrlCmdGpuSetPartitions_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetPartitions -- exported (id=0x20800175)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__subdeviceCtrlCmdGpuGetPartitions__ = &subdeviceCtrlCmdGpuGetPartitions_IMPL;
#endif

    // subdeviceCtrlCmdGpuGetComputeProfiles -- exported (id=0x208001a2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__subdeviceCtrlCmdGpuGetComputeProfiles__ = &subdeviceCtrlCmdGpuGetComputeProfiles_IMPL;
#endif

    // subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles -- exported (id=0x20800a63)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL;
#endif

    // subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines -- exported (id=0x20800a65)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL;
#endif

    // subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges -- exported (id=0x20800a66)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL;
#endif

    // subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles -- exported (id=0x20800aba)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_IMPL;
#endif

    // subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance -- exported (id=0x20800aa7)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
    pThis->__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__ = &subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL;
#endif

    // subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance -- exported (id=0x20800aa9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
    pThis->__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__ = &subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL;
#endif

    // subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange -- exported (id=0x20800a44)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange__ = &subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_IMPL;
#endif

    // subdeviceCtrlCmdOsUnixGc6BlockerRefCnt -- exported (id=0x20803d01)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
    pThis->__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__ = &subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL;
#endif

    // subdeviceCtrlCmdOsUnixAllowDisallowGcoff -- exported (id=0x20803d02)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__ = &subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL;
#endif

    // subdeviceCtrlCmdOsUnixAudioDynamicPower -- exported (id=0x20803d03)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
    pThis->__subdeviceCtrlCmdOsUnixAudioDynamicPower__ = &subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL;
#endif

    // subdeviceCtrlCmdOsUnixVidmemPersistenceStatus -- exported (id=0x20803d07)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xbu)
    pThis->__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__ = &subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL;
#endif

    // subdeviceCtrlCmdOsUnixUpdateTgpStatus -- exported (id=0x20803d08)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdOsUnixUpdateTgpStatus__ = &subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL;
#endif

    // subdeviceCtrlCmdDisplayGetIpVersion -- exported (id=0x20800a4b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayGetIpVersion__ = &subdeviceCtrlCmdDisplayGetIpVersion_IMPL;
#endif

    // subdeviceCtrlCmdDisplayGetStaticInfo -- exported (id=0x20800a01)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayGetStaticInfo__ = &subdeviceCtrlCmdDisplayGetStaticInfo_IMPL;
#endif

    // subdeviceCtrlCmdDisplaySetChannelPushbuffer -- exported (id=0x20800a58)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplaySetChannelPushbuffer__ = &subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL;
#endif

    // subdeviceCtrlCmdDisplayWriteInstMem -- exported (id=0x20800a49)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayWriteInstMem__ = &subdeviceCtrlCmdDisplayWriteInstMem_IMPL;
#endif

    // subdeviceCtrlCmdDisplaySetupRgLineIntr -- exported (id=0x20800a4d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplaySetupRgLineIntr__ = &subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL;
#endif

    // subdeviceCtrlCmdDisplaySetImportedImpData -- exported (id=0x20800a54)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplaySetImportedImpData__ = &subdeviceCtrlCmdDisplaySetImportedImpData_IMPL;
#endif

    // subdeviceCtrlCmdDisplayGetDisplayMask -- exported (id=0x20800a5d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayGetDisplayMask__ = &subdeviceCtrlCmdDisplayGetDisplayMask_IMPL;
#endif

    // subdeviceCtrlCmdDisplayPinsetsToLockpins -- exported (id=0x20800adc)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayPinsetsToLockpins__ = &subdeviceCtrlCmdDisplayPinsetsToLockpins_IMPL;
#endif

    // subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl -- exported (id=0x20800ade)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl__ = &subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpioProgramDirection -- exported (id=0x20802300)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGpioProgramDirection__ = &subdeviceCtrlCmdInternalGpioProgramDirection_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpioProgramOutput -- exported (id=0x20802301)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGpioProgramOutput__ = &subdeviceCtrlCmdInternalGpioProgramOutput_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpioReadInput -- exported (id=0x20802302)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGpioReadInput__ = &subdeviceCtrlCmdInternalGpioReadInput_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpioActivateHwFunction -- exported (id=0x20802303)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGpioActivateHwFunction__ = &subdeviceCtrlCmdInternalGpioActivateHwFunction_IMPL;
#endif

    // subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated -- exported (id=0x20800af0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated__ = &subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_IMPL;
#endif

    // subdeviceCtrlCmdInternalDisplayPreModeSet -- exported (id=0x20800af1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalDisplayPreModeSet__ = &subdeviceCtrlCmdInternalDisplayPreModeSet_IMPL;
#endif

    // subdeviceCtrlCmdInternalDisplayPostModeSet -- exported (id=0x20800af2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalDisplayPostModeSet__ = &subdeviceCtrlCmdInternalDisplayPostModeSet_IMPL;
#endif

    // subdeviceCtrlCmdDisplayPreUnixConsole -- exported (id=0x20800a76)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayPreUnixConsole__ = &subdeviceCtrlCmdDisplayPreUnixConsole_IMPL;
#endif

    // subdeviceCtrlCmdDisplayPostUnixConsole -- exported (id=0x20800a77)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdDisplayPostUnixConsole__ = &subdeviceCtrlCmdDisplayPostUnixConsole_IMPL;
#endif

    // subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer -- exported (id=0x20800a1d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__ = &subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL;
#endif

    // subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer -- exported (id=0x20800a1e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__ = &subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL;
#endif

    // subdeviceCtrlCmdInternalGetChipInfo -- exported (id=0x20800a36)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x404c0u)
    pThis->__subdeviceCtrlCmdInternalGetChipInfo__ = &subdeviceCtrlCmdInternalGetChipInfo_IMPL;
#endif

    // subdeviceCtrlCmdInternalGetUserRegisterAccessMap -- exported (id=0x20800a41)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
    pThis->__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__ = &subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL;
#endif

    // subdeviceCtrlCmdInternalGetDeviceInfoTable -- exported (id=0x20800a40)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c4c0u)
    pThis->__subdeviceCtrlCmdInternalGetDeviceInfoTable__ = &subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL;
#endif

    // subdeviceCtrlCmdInternalRecoverAllComputeContexts -- exported (id=0x20800a4a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalRecoverAllComputeContexts__ = &subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL;
#endif

    // subdeviceCtrlCmdInternalGetSmcMode -- exported (id=0x20800a4c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGetSmcMode__ = &subdeviceCtrlCmdInternalGetSmcMode_IMPL;
#endif

    // subdeviceCtrlCmdIsEgpuBridge -- exported (id=0x20800a55)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdIsEgpuBridge__ = &subdeviceCtrlCmdIsEgpuBridge_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap -- exported (id=0x20800aeb)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap__ = &subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_IMPL;
#endif

    // subdeviceCtrlCmdInternalBusFlushWithSysmembar -- exported (id=0x20800a70)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalBusFlushWithSysmembar__ = &subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL;
#endif
} // End __nvoc_init_funcTable_Subdevice_2 with approximately 263 basic block(s).

// Vtable initialization 3/3
static void __nvoc_init_funcTable_Subdevice_3(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal -- exported (id=0x20800a71)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__ = &subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL;
#endif

    // subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote -- exported (id=0x20800a72)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__ = &subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL;
#endif

    // subdeviceCtrlCmdInternalBusDestroyP2pMailbox -- exported (id=0x20800a73)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__ = &subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL;
#endif

    // subdeviceCtrlCmdInternalBusCreateC2cPeerMapping -- exported (id=0x20800a74)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__ = &subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL;
#endif

    // subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping -- exported (id=0x20800a75)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__ = &subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL;
#endif

    // subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries -- exported (id=0x20800a57)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries__ = &subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_IMPL;
#endif

    // subdeviceCtrlCmdGmmuGetStaticInfo -- exported (id=0x20800a59)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdGmmuGetStaticInfo__ = &subdeviceCtrlCmdGmmuGetStaticInfo_IMPL;
#endif

    // subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer -- exported (id=0x20800a9b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL;
#endif

    // subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer -- exported (id=0x20800a9c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c0u)
    pThis->__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL;
#endif

    // subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer -- exported (id=0x20800a9d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL;
#endif

    // subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer -- exported (id=0x20800a9e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL;
#endif

    // subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer -- exported (id=0x20800a9f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer__ = &subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_IMPL;
#endif

    // subdeviceCtrlCmdCeGetPhysicalCaps -- exported (id=0x20802a07)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101d0u)
    pThis->__subdeviceCtrlCmdCeGetPhysicalCaps__ = &subdeviceCtrlCmdCeGetPhysicalCaps_IMPL;
#endif

    // subdeviceCtrlCmdCeGetAllPhysicalCaps -- exported (id=0x20802a0b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
    pThis->__subdeviceCtrlCmdCeGetAllPhysicalCaps__ = &subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL;
#endif

    // subdeviceCtrlCmdCeUpdateClassDB -- exported (id=0x20802a06)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdCeUpdateClassDB__ = &subdeviceCtrlCmdCeUpdateClassDB_IMPL;
#endif

    // subdeviceCtrlCmdCeGetFaultMethodBufferSize -- exported (id=0x20802a08)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c040u)
    pThis->__subdeviceCtrlCmdCeGetFaultMethodBufferSize__ = &subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL;
#endif

    // subdeviceCtrlCmdCeGetHubPceMask -- exported (id=0x20802a09)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4c0u)
    pThis->__subdeviceCtrlCmdCeGetHubPceMask__ = &subdeviceCtrlCmdCeGetHubPceMask_IMPL;
#endif

    // subdeviceCtrlCmdCeGetHubPceMaskV2 -- exported (id=0x20802a0e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdCeGetHubPceMaskV2__ = &subdeviceCtrlCmdCeGetHubPceMaskV2_IMPL;
#endif

    // subdeviceCtrlCmdIntrGetKernelTable -- exported (id=0x20800a5c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdIntrGetKernelTable__ = &subdeviceCtrlCmdIntrGetKernelTable_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfCudaLimitDisable -- exported (id=0x20800a7a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalPerfCudaLimitDisable__ = &subdeviceCtrlCmdInternalPerfCudaLimitDisable_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfOptpCliClear -- exported (id=0x20800a7c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfOptpCliClear__ = &subdeviceCtrlCmdInternalPerfOptpCliClear_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfBoostSet_2x -- exported (id=0x20800a9a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalPerfBoostSet_2x__ = &subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfBoostSet_3x -- exported (id=0x20800aa0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalPerfBoostSet_3x__ = &subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfBoostClear_3x -- exported (id=0x20800aa1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalPerfBoostClear_3x__ = &subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl -- exported (id=0x20800a7e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__ = &subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo -- exported (id=0x20800a80)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__ = &subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits -- exported (id=0x20800a7f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__ = &subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck -- exported (id=0x20800a98)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__ = &subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet -- exported (id=0x20800a99)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__ = &subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount -- exported (id=0x20800ab1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__ = &subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfGetAuxPowerState -- exported (id=0x20800a81)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfGetAuxPowerState__ = &subdeviceCtrlCmdInternalPerfGetAuxPowerState_IMPL;
#endif

    // subdeviceCtrlCmdInternalGcxEntryPrerequisite -- exported (id=0x2080a7d7)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGcxEntryPrerequisite__ = &subdeviceCtrlCmdInternalGcxEntryPrerequisite_IMPL;
#endif

    // subdeviceCtrlCmdBifGetStaticInfo -- halified (2 hals) exported (id=0x20800aac) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdBifGetStaticInfo__ = &subdeviceCtrlCmdBifGetStaticInfo_VF;
    }
    else
    {
        pThis->__subdeviceCtrlCmdBifGetStaticInfo__ = &subdeviceCtrlCmdBifGetStaticInfo_92bfc3;
    }

    // subdeviceCtrlCmdBifGetAspmL1Flags -- exported (id=0x20800ab0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d0u)
    pThis->__subdeviceCtrlCmdBifGetAspmL1Flags__ = &subdeviceCtrlCmdBifGetAspmL1Flags_IMPL;
#endif

    // subdeviceCtrlCmdBifSetPcieRo -- exported (id=0x20800ab9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdBifSetPcieRo__ = &subdeviceCtrlCmdBifSetPcieRo_IMPL;
#endif

    // subdeviceCtrlCmdBifDisableSystemMemoryAccess -- exported (id=0x20800adb)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdBifDisableSystemMemoryAccess__ = &subdeviceCtrlCmdBifDisableSystemMemoryAccess_IMPL;
#endif

    // subdeviceCtrlCmdHshubPeerConnConfig -- exported (id=0x20800a88)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubPeerConnConfig__ = &subdeviceCtrlCmdHshubPeerConnConfig_IMPL;
#endif

    // subdeviceCtrlCmdHshubFirstLinkPeerId -- exported (id=0x20800a89)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubFirstLinkPeerId__ = &subdeviceCtrlCmdHshubFirstLinkPeerId_IMPL;
#endif

    // subdeviceCtrlCmdHshubGetHshubIdForLinks -- exported (id=0x20800a8a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubGetHshubIdForLinks__ = &subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL;
#endif

    // subdeviceCtrlCmdHshubGetNumUnits -- exported (id=0x20800a8b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubGetNumUnits__ = &subdeviceCtrlCmdHshubGetNumUnits_IMPL;
#endif

    // subdeviceCtrlCmdHshubNextHshubId -- exported (id=0x20800a8c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubNextHshubId__ = &subdeviceCtrlCmdHshubNextHshubId_IMPL;
#endif

    // subdeviceCtrlCmdHshubGetMaxHshubsPerShim -- exported (id=0x20800a79)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubGetMaxHshubsPerShim__ = &subdeviceCtrlCmdHshubGetMaxHshubsPerShim_IMPL;
#endif

    // subdeviceCtrlCmdHshubEgmConfig -- exported (id=0x20800a8d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdHshubEgmConfig__ = &subdeviceCtrlCmdHshubEgmConfig_IMPL;
#endif

    // subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr -- halified (2 hals) exported (id=0x20800aad) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__ = &subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__ = &subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_92bfc3;
    }

    // subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr -- exported (id=0x20800aae)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__ = &subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL;
#endif

    // subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl -- exported (id=0x20800ac7)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
    pThis->__subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl__ = &subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_IMPL;
#endif

    // subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl -- exported (id=0x20800ac8)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
    pThis->__subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl__ = &subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_IMPL;
#endif

    // subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery -- exported (id=0x20800aea)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100c0u)
    pThis->__subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery__ = &subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_IMPL;
#endif

    // subdeviceCtrlCmdInternalSetP2pCaps -- exported (id=0x20800ab5)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalSetP2pCaps__ = &subdeviceCtrlCmdInternalSetP2pCaps_IMPL;
#endif

    // subdeviceCtrlCmdInternalRemoveP2pCaps -- exported (id=0x20800ab6)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalRemoveP2pCaps__ = &subdeviceCtrlCmdInternalRemoveP2pCaps_IMPL;
#endif

    // subdeviceCtrlCmdInternalGetPcieP2pCaps -- exported (id=0x20800ab8)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGetPcieP2pCaps__ = &subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL;
#endif

    // subdeviceCtrlCmdInternalGetLocalAtsConfig -- halified (2 hals) exported (id=0x20800afb) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdInternalGetLocalAtsConfig__ = &subdeviceCtrlCmdInternalGetLocalAtsConfig_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdInternalGetLocalAtsConfig__ = &subdeviceCtrlCmdInternalGetLocalAtsConfig_92bfc3;
    }

    // subdeviceCtrlCmdInternalSetPeerAtsConfig -- halified (2 hals) exported (id=0x20800afc) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__subdeviceCtrlCmdInternalSetPeerAtsConfig__ = &subdeviceCtrlCmdInternalSetPeerAtsConfig_46f6a7;
    }
    else
    {
        pThis->__subdeviceCtrlCmdInternalSetPeerAtsConfig__ = &subdeviceCtrlCmdInternalSetPeerAtsConfig_92bfc3;
    }

    // subdeviceCtrlCmdInternalInitGpuIntr -- exported (id=0x20800abe)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalInitGpuIntr__ = &subdeviceCtrlCmdInternalInitGpuIntr_IMPL;
#endif

    // subdeviceCtrlCmdInternalGsyncOptimizeTiming -- exported (id=0x20800abf)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGsyncOptimizeTiming__ = &subdeviceCtrlCmdInternalGsyncOptimizeTiming_IMPL;
#endif

    // subdeviceCtrlCmdInternalGsyncGetDisplayIds -- exported (id=0x20800ac0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGsyncGetDisplayIds__ = &subdeviceCtrlCmdInternalGsyncGetDisplayIds_IMPL;
#endif

    // subdeviceCtrlCmdInternalGsyncSetStereoSync -- exported (id=0x20800ac1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGsyncSetStereoSync__ = &subdeviceCtrlCmdInternalGsyncSetStereoSync_IMPL;
#endif

    // subdeviceCtrlCmdInternalGsyncGetVactiveLines -- exported (id=0x20800ac4)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGsyncGetVactiveLines__ = &subdeviceCtrlCmdInternalGsyncGetVactiveLines_IMPL;
#endif

    // subdeviceCtrlCmdInternalGsyncIsDisplayIdValid -- exported (id=0x20800ac9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGsyncIsDisplayIdValid__ = &subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_IMPL;
#endif

    // subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync -- exported (id=0x20800aca)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync__ = &subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_IMPL;
#endif

    // subdeviceCtrlCmdInternalFbsrInit -- exported (id=0x20800ac2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalFbsrInit__ = &subdeviceCtrlCmdInternalFbsrInit_IMPL;
#endif

    // subdeviceCtrlCmdInternalPostInitBrightcStateLoad -- exported (id=0x20800ac6)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPostInitBrightcStateLoad__ = &subdeviceCtrlCmdInternalPostInitBrightcStateLoad_IMPL;
#endif

    // subdeviceCtrlCmdInternalSetStaticEdidData -- exported (id=0x20800adf)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalSetStaticEdidData__ = &subdeviceCtrlCmdInternalSetStaticEdidData_IMPL;
#endif

    // subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate -- exported (id=0x20800acb)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate__ = &subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_IMPL;
#endif

    // subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync -- exported (id=0x20800acc)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync__ = &subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_IMPL;
#endif

    // subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync -- exported (id=0x20800acd)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync__ = &subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State -- exported (id=0x20800ace)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State__ = &subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State -- exported (id=0x20800acf)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State__ = &subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_IMPL;
#endif

    // subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit -- exported (id=0x20800ad0)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit__ = &subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_IMPL;
#endif

    // subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo -- exported (id=0x20800afd)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo__ = &subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_IMPL;
#endif

    // subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit -- exported (id=0x20800ad1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit__ = &subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_IMPL;
#endif

    // subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode -- exported (id=0x20800ad2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode__ = &subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_IMPL;
#endif

    // subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2 -- exported (id=0x20800ad3)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2__ = &subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo -- exported (id=0x20800ad4)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo__ = &subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping -- exported (id=0x20800ad5)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping__ = &subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_IMPL;
#endif

    // subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate -- exported (id=0x20800ad6)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate__ = &subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_IMPL;
#endif

    // subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit -- exported (id=0x20800a7b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit__ = &subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_IMPL;
#endif

    // subdeviceCtrlCmdInternalDetectHsVideoBridge -- exported (id=0x20800add)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalDetectHsVideoBridge__ = &subdeviceCtrlCmdInternalDetectHsVideoBridge_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeGetStaticInfo -- exported (id=0x20800af3)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeGetStaticInfo__ = &subdeviceCtrlCmdInternalConfComputeGetStaticInfo_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys -- exported (id=0x20800ae1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys__ = &subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeDeriveLceKeys -- exported (id=0x20800ae2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeDeriveLceKeys__ = &subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeRotateKeys -- exported (id=0x20800ae5)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeRotateKeys__ = &subdeviceCtrlCmdInternalConfComputeRotateKeys_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation -- exported (id=0x20800ae6)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation__ = &subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeSetGpuState -- exported (id=0x20800ae7)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeSetGpuState__ = &subdeviceCtrlCmdInternalConfComputeSetGpuState_IMPL;
#endif

    // subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy -- exported (id=0x20800ae8)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy__ = &subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_IMPL;
#endif

    // subdeviceCtrlCmdInternalInitUserSharedData -- exported (id=0x20800afe)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalInitUserSharedData__ = &subdeviceCtrlCmdInternalInitUserSharedData_IMPL;
#endif

    // subdeviceCtrlCmdInternalUserSharedDataSetDataPoll -- exported (id=0x20800aff)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdInternalUserSharedDataSetDataPoll__ = &subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_IMPL;
#endif

    // subdeviceCtrlCmdInternalGspStartTrace -- exported (id=0x208001e3)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xd0u)
    pThis->__subdeviceCtrlCmdInternalGspStartTrace__ = &subdeviceCtrlCmdInternalGspStartTrace_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter -- exported (id=0x20800ae9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter__ = &subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_IMPL;
#endif

    // subdeviceCtrlCmdInternalGpuSetIllum -- exported (id=0x20800aec)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__subdeviceCtrlCmdInternalGpuSetIllum__ = &subdeviceCtrlCmdInternalGpuSetIllum_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask -- exported (id=0x20804001)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask__ = &subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask -- exported (id=0x20804002)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask__ = &subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType -- exported (id=0x20804003)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType__ = &subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu -- exported (id=0x20804004)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu__ = &subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo -- exported (id=0x20804005)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo__ = &subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage -- exported (id=0x20804006)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage__ = &subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity -- exported (id=0x20804007)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity__ = &subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources -- exported (id=0x20804008)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources__ = &subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding -- exported (id=0x20804009)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding__ = &subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport -- exported (id=0x2080400a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport__ = &subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig -- exported (id=0x2080400b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig__ = &subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_IMPL;
#endif

    // subdeviceCtrlCmdVgpuMgrInternalFreeStates -- exported (id=0x2080400c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdVgpuMgrInternalFreeStates__ = &subdeviceCtrlCmdVgpuMgrInternalFreeStates_IMPL;
#endif

    // subdeviceCtrlCmdGetAvailableHshubMask -- exported (id=0x20804101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
    pThis->__subdeviceCtrlCmdGetAvailableHshubMask__ = &subdeviceCtrlCmdGetAvailableHshubMask_IMPL;
#endif

    // subdeviceCtrlSetEcThrottleMode -- exported (id=0x20804102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x158u)
    pThis->__subdeviceCtrlSetEcThrottleMode__ = &subdeviceCtrlSetEcThrottleMode_IMPL;
#endif

    // subdeviceCtrlCmdCcuMap -- exported (id=0x20800ab3)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdCcuMap__ = &subdeviceCtrlCmdCcuMap_IMPL;
#endif

    // subdeviceCtrlCmdCcuUnmap -- exported (id=0x20800ab4)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdCcuUnmap__ = &subdeviceCtrlCmdCcuUnmap_IMPL;
#endif

    // subdeviceCtrlCmdCcuSetStreamState -- exported (id=0x20800abd)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdCcuSetStreamState__ = &subdeviceCtrlCmdCcuSetStreamState_IMPL;
#endif

    // subdeviceCtrlCmdCcuGetSampleInfo -- exported (id=0x20800ab2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdCcuGetSampleInfo__ = &subdeviceCtrlCmdCcuGetSampleInfo_IMPL;
#endif

    // subdeviceCtrlCmdSpdmPartition -- exported (id=0x20800ad9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__subdeviceCtrlCmdSpdmPartition__ = &subdeviceCtrlCmdSpdmPartition_IMPL;
#endif

    // subdeviceControl -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceControl__ = &__nvoc_up_thunk_GpuResource_subdeviceControl;

    // subdeviceMap -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceMap__ = &__nvoc_up_thunk_GpuResource_subdeviceMap;

    // subdeviceUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceUnmap__ = &__nvoc_up_thunk_GpuResource_subdeviceUnmap;

    // subdeviceShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceShareCallback__ = &__nvoc_up_thunk_GpuResource_subdeviceShareCallback;

    // subdeviceGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize;

    // subdeviceGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_subdeviceGetMapAddrSpace;

    // subdeviceGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__subdeviceGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_subdeviceGetInternalObjectHandle;

    // subdeviceAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceAccessCallback__ = &__nvoc_up_thunk_RmResource_subdeviceAccessCallback;

    // subdeviceGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_subdeviceGetMemInterMapParams;

    // subdeviceCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_subdeviceCheckMemInterUnmap;

    // subdeviceGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_subdeviceGetMemoryMappingDescriptor;

    // subdeviceControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_subdeviceControlSerialization_Prologue;

    // subdeviceControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_subdeviceControlSerialization_Epilogue;

    // subdeviceControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceControl_Prologue__ = &__nvoc_up_thunk_RmResource_subdeviceControl_Prologue;

    // subdeviceControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__subdeviceControl_Epilogue__ = &__nvoc_up_thunk_RmResource_subdeviceControl_Epilogue;

    // subdeviceCanCopy -- virtual inherited (res) base (gpures)
    pThis->__subdeviceCanCopy__ = &__nvoc_up_thunk_RsResource_subdeviceCanCopy;

    // subdeviceIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__subdeviceIsDuplicate__ = &__nvoc_up_thunk_RsResource_subdeviceIsDuplicate;

    // subdeviceControlFilter -- virtual inherited (res) base (gpures)
    pThis->__subdeviceControlFilter__ = &__nvoc_up_thunk_RsResource_subdeviceControlFilter;

    // subdeviceIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__subdeviceIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_subdeviceIsPartialUnmapSupported;

    // subdeviceMapTo -- virtual inherited (res) base (gpures)
    pThis->__subdeviceMapTo__ = &__nvoc_up_thunk_RsResource_subdeviceMapTo;

    // subdeviceUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__subdeviceUnmapFrom__ = &__nvoc_up_thunk_RsResource_subdeviceUnmapFrom;

    // subdeviceGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__subdeviceGetRefCount__ = &__nvoc_up_thunk_RsResource_subdeviceGetRefCount;

    // subdeviceAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__subdeviceAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_subdeviceAddAdditionalDependants;

    // subdeviceGetNotificationListPtr -- virtual inherited (notify) base (notify)
    pThis->__subdeviceGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_subdeviceGetNotificationListPtr;

    // subdeviceGetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__subdeviceGetNotificationShare__ = &__nvoc_up_thunk_Notifier_subdeviceGetNotificationShare;

    // subdeviceSetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__subdeviceSetNotificationShare__ = &__nvoc_up_thunk_Notifier_subdeviceSetNotificationShare;

    // subdeviceUnregisterEvent -- virtual inherited (notify) base (notify)
    pThis->__subdeviceUnregisterEvent__ = &__nvoc_up_thunk_Notifier_subdeviceUnregisterEvent;

    // subdeviceGetOrAllocNotifShare -- virtual inherited (notify) base (notify)
    pThis->__subdeviceGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_subdeviceGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_Subdevice_3 with approximately 141 basic block(s).


// Initialize vtable(s) for 621 virtual method(s).
void __nvoc_init_funcTable_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 621 per-object function pointer(s).
    // To reduce stack pressure with some unoptimized builds, the logic is distributed among 3 functions.
    __nvoc_init_funcTable_Subdevice_1(pThis, pRmhalspecowner);
    __nvoc_init_funcTable_Subdevice_2(pThis, pRmhalspecowner);
    __nvoc_init_funcTable_Subdevice_3(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_Subdevice = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_Subdevice(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Subdevice(Subdevice **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Subdevice *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Subdevice), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Subdevice));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Subdevice);

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

    __nvoc_init_Subdevice(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Subdevice(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Subdevice_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Subdevice_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Subdevice));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Subdevice(Subdevice **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Subdevice(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

