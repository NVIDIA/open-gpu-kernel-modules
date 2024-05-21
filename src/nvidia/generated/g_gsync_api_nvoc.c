#define NVOC_GSYNC_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gsync_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x214628 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GSyncApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_GSyncApi(GSyncApi*);
void __nvoc_init_funcTable_GSyncApi(GSyncApi*);
NV_STATUS __nvoc_ctor_GSyncApi(GSyncApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GSyncApi(GSyncApi*);
void __nvoc_dtor_GSyncApi(GSyncApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GSyncApi;

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_GSyncApi = {
    /*pClassDef=*/          &__nvoc_class_def_GSyncApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GSyncApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GSyncApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_GSyncApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GSyncApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GSyncApi = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_GSyncApi_GSyncApi,
        &__nvoc_rtti_GSyncApi_Notifier,
        &__nvoc_rtti_GSyncApi_INotifier,
        &__nvoc_rtti_GSyncApi_RmResource,
        &__nvoc_rtti_GSyncApi_RmResourceCommon,
        &__nvoc_rtti_GSyncApi_RsResource,
        &__nvoc_rtti_GSyncApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GSyncApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GSyncApi),
        /*classId=*/            classId(GSyncApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GSyncApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GSyncApi,
    /*pCastInfo=*/          &__nvoc_castinfo_GSyncApi,
    /*pExportInfo=*/        &__nvoc_export_info_GSyncApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GSyncApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10101u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetVersion"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10102u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetStatusSignals"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlParams_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10103u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlParams"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlParams_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10104u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlParams"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10105u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetCaps"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10106u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGetGsyncGpuTopology"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10110u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlSync"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10111u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlSync"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10112u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlUnsync"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetStatusSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10113u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetStatusSync"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10114u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetStatus"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlTesting_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10120u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlTesting"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlTesting_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10121u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlTesting"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10130u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlWatchdog"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10140u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlInterlaceMode"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10141u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlInterlaceMode"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10150u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlSwapBarrier"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10151u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlSwapBarrier"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10153u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlSwapLockWindow"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10160u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetOptimizedTiming"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetEventNotification_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10170u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetEventNotification"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10172u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlStereoLockMode"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10173u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlStereoLockMode"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncReadRegister_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10180u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncReadRegister"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncWriteRegister_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10181u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncWriteRegister"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetLocalSync_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10185u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetLocalSync"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncConfigFlash_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10186u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncConfigFlash"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10187u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetHouseSyncMode"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10188u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetHouseSyncMode"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in GSyncApi from superclasses

// gsyncapiControl: virtual override (res) base (rmres)
static NV_STATUS __nvoc_down_thunk_GSyncApi_resControl(struct RsResource *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gsyncapiControl((struct GSyncApi *)(((unsigned char *) pGsyncApi) - __nvoc_rtti_GSyncApi_RsResource.offset), pCallContext, pParams);
}


// 25 up-thunk(s) defined to bridge methods in GSyncApi to superclasses

// gsyncapiAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_gsyncapiAccessCallback(struct GSyncApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// gsyncapiShareCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_gsyncapiShareCallback(struct GSyncApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// gsyncapiGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gsyncapiGetMemInterMapParams(struct GSyncApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pParams);
}

// gsyncapiCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gsyncapiCheckMemInterUnmap(struct GSyncApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GSyncApi_RmResource.offset), bSubdeviceHandleProvided);
}

// gsyncapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gsyncapiGetMemoryMappingDescriptor(struct GSyncApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GSyncApi_RmResource.offset), ppMemDesc);
}

// gsyncapiControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gsyncapiControlSerialization_Prologue(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pCallContext, pParams);
}

// gsyncapiControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_gsyncapiControlSerialization_Epilogue(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pCallContext, pParams);
}

// gsyncapiControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gsyncapiControl_Prologue(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pCallContext, pParams);
}

// gsyncapiControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_gsyncapiControl_Epilogue(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RmResource.offset), pCallContext, pParams);
}

// gsyncapiCanCopy: virtual inherited (res) base (rmres)
static NvBool __nvoc_up_thunk_RsResource_gsyncapiCanCopy(struct GSyncApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset));
}

// gsyncapiIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gsyncapiIsDuplicate(struct GSyncApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), hMemory, pDuplicate);
}

// gsyncapiPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_gsyncapiPreDestruct(struct GSyncApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset));
}

// gsyncapiControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gsyncapiControlFilter(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), pCallContext, pParams);
}

// gsyncapiMap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gsyncapiMap(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// gsyncapiUnmap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gsyncapiUnmap(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), pCallContext, pCpuMapping);
}

// gsyncapiIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_gsyncapiIsPartialUnmapSupported(struct GSyncApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset));
}

// gsyncapiMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gsyncapiMapTo(struct GSyncApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), pParams);
}

// gsyncapiUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gsyncapiUnmapFrom(struct GSyncApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), pParams);
}

// gsyncapiGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_gsyncapiGetRefCount(struct GSyncApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset));
}

// gsyncapiAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_gsyncapiAddAdditionalDependants(struct RsClient *pClient, struct GSyncApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GSyncApi_RsResource.offset), pReference);
}

// gsyncapiGetNotificationListPtr: virtual inherited (notify) base (notify)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_gsyncapiGetNotificationListPtr(struct GSyncApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_GSyncApi_Notifier.offset));
}

// gsyncapiGetNotificationShare: virtual inherited (notify) base (notify)
static struct NotifShare * __nvoc_up_thunk_Notifier_gsyncapiGetNotificationShare(struct GSyncApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_GSyncApi_Notifier.offset));
}

// gsyncapiSetNotificationShare: virtual inherited (notify) base (notify)
static void __nvoc_up_thunk_Notifier_gsyncapiSetNotificationShare(struct GSyncApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_GSyncApi_Notifier.offset), pNotifShare);
}

// gsyncapiUnregisterEvent: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_gsyncapiUnregisterEvent(struct GSyncApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_GSyncApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// gsyncapiGetOrAllocNotifShare: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_gsyncapiGetOrAllocNotifShare(struct GSyncApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_GSyncApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_GSyncApi = 
{
    /*numEntries=*/     29,
    /*pExportEntries=*/ __nvoc_exported_method_def_GSyncApi
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_GSyncApi(GSyncApi *pThis) {
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GSyncApi(GSyncApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_GSyncApi(GSyncApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GSyncApi_fail_RmResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_GSyncApi_fail_Notifier;
    __nvoc_init_dataField_GSyncApi(pThis);

    status = __nvoc_gsyncapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GSyncApi_fail__init;
    goto __nvoc_ctor_GSyncApi_exit; // Success

__nvoc_ctor_GSyncApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_GSyncApi_fail_Notifier:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_GSyncApi_fail_RmResource:
__nvoc_ctor_GSyncApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GSyncApi_1(GSyncApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // gsyncapiControl -- virtual override (res) base (rmres)
    pThis->__gsyncapiControl__ = &gsyncapiControl_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__ = &__nvoc_down_thunk_GSyncApi_resControl;

    // gsyncapiCtrlCmdGsyncGetVersion -- exported (id=0x30f10101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetVersion__ = &gsyncapiCtrlCmdGsyncGetVersion_IMPL;
#endif

    // gsyncapiCtrlCmdGetGsyncGpuTopology -- exported (id=0x30f10106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGetGsyncGpuTopology__ = &gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetStatusSignals -- exported (id=0x30f10102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetStatusSignals__ = &gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlParams -- exported (id=0x30f10103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlParams__ = &gsyncapiCtrlCmdGsyncGetControlParams_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlParams -- exported (id=0x30f10104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlParams__ = &gsyncapiCtrlCmdGsyncSetControlParams_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlSync -- exported (id=0x30f10110)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlSync__ = &gsyncapiCtrlCmdGsyncGetControlSync_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlSync -- exported (id=0x30f10111)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlSync__ = &gsyncapiCtrlCmdGsyncSetControlSync_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlUnsync -- exported (id=0x30f10112)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlUnsync__ = &gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetStatusSync -- exported (id=0x30f10113)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetStatusSync__ = &gsyncapiCtrlCmdGsyncGetStatusSync_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetStatus -- exported (id=0x30f10114)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetStatus__ = &gsyncapiCtrlCmdGsyncGetStatus_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlTesting -- exported (id=0x30f10120)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlTesting__ = &gsyncapiCtrlCmdGsyncGetControlTesting_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlTesting -- exported (id=0x30f10121)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlTesting__ = &gsyncapiCtrlCmdGsyncSetControlTesting_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlWatchdog -- exported (id=0x30f10130)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlWatchdog__ = &gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlInterlaceMode -- exported (id=0x30f10140)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlInterlaceMode__ = &gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlInterlaceMode -- exported (id=0x30f10141)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlInterlaceMode__ = &gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlSwapBarrier -- exported (id=0x30f10150)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlSwapBarrier__ = &gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlSwapBarrier -- exported (id=0x30f10151)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlSwapBarrier__ = &gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlSwapLockWindow -- exported (id=0x30f10153)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlSwapLockWindow__ = &gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetCaps -- exported (id=0x30f10105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetCaps__ = &gsyncapiCtrlCmdGsyncGetCaps_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetOptimizedTiming -- exported (id=0x30f10160)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetOptimizedTiming__ = &gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetEventNotification -- exported (id=0x30f10170)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncSetEventNotification__ = &gsyncapiCtrlCmdGsyncSetEventNotification_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetControlStereoLockMode -- exported (id=0x30f10173)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetControlStereoLockMode__ = &gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetControlStereoLockMode -- exported (id=0x30f10172)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetControlStereoLockMode__ = &gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncReadRegister -- exported (id=0x30f10180)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncReadRegister__ = &gsyncapiCtrlCmdGsyncReadRegister_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncWriteRegister -- exported (id=0x30f10181)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncWriteRegister__ = &gsyncapiCtrlCmdGsyncWriteRegister_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetLocalSync -- exported (id=0x30f10185)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetLocalSync__ = &gsyncapiCtrlCmdGsyncSetLocalSync_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncConfigFlash -- exported (id=0x30f10186)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncConfigFlash__ = &gsyncapiCtrlCmdGsyncConfigFlash_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncGetHouseSyncMode -- exported (id=0x30f10187)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gsyncapiCtrlCmdGsyncGetHouseSyncMode__ = &gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL;
#endif

    // gsyncapiCtrlCmdGsyncSetHouseSyncMode -- exported (id=0x30f10188)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__gsyncapiCtrlCmdGsyncSetHouseSyncMode__ = &gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL;
#endif

    // gsyncapiAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiAccessCallback__ = &__nvoc_up_thunk_RmResource_gsyncapiAccessCallback;

    // gsyncapiShareCallback -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiShareCallback__ = &__nvoc_up_thunk_RmResource_gsyncapiShareCallback;

    // gsyncapiGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gsyncapiGetMemInterMapParams;

    // gsyncapiCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gsyncapiCheckMemInterUnmap;

    // gsyncapiGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gsyncapiGetMemoryMappingDescriptor;

    // gsyncapiControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gsyncapiControlSerialization_Prologue;

    // gsyncapiControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gsyncapiControlSerialization_Epilogue;

    // gsyncapiControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_gsyncapiControl_Prologue;

    // gsyncapiControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__gsyncapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gsyncapiControl_Epilogue;

    // gsyncapiCanCopy -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiCanCopy__ = &__nvoc_up_thunk_RsResource_gsyncapiCanCopy;

    // gsyncapiIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_gsyncapiIsDuplicate;

    // gsyncapiPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiPreDestruct__ = &__nvoc_up_thunk_RsResource_gsyncapiPreDestruct;

    // gsyncapiControlFilter -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiControlFilter__ = &__nvoc_up_thunk_RsResource_gsyncapiControlFilter;

    // gsyncapiMap -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiMap__ = &__nvoc_up_thunk_RsResource_gsyncapiMap;

    // gsyncapiUnmap -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiUnmap__ = &__nvoc_up_thunk_RsResource_gsyncapiUnmap;

    // gsyncapiIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__gsyncapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gsyncapiIsPartialUnmapSupported;

    // gsyncapiMapTo -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiMapTo__ = &__nvoc_up_thunk_RsResource_gsyncapiMapTo;

    // gsyncapiUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_gsyncapiUnmapFrom;

    // gsyncapiGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiGetRefCount__ = &__nvoc_up_thunk_RsResource_gsyncapiGetRefCount;

    // gsyncapiAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__gsyncapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gsyncapiAddAdditionalDependants;

    // gsyncapiGetNotificationListPtr -- virtual inherited (notify) base (notify)
    pThis->__gsyncapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_gsyncapiGetNotificationListPtr;

    // gsyncapiGetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__gsyncapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_gsyncapiGetNotificationShare;

    // gsyncapiSetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__gsyncapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_gsyncapiSetNotificationShare;

    // gsyncapiUnregisterEvent -- virtual inherited (notify) base (notify)
    pThis->__gsyncapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_gsyncapiUnregisterEvent;

    // gsyncapiGetOrAllocNotifShare -- virtual inherited (notify) base (notify)
    pThis->__gsyncapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_gsyncapiGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_GSyncApi_1 with approximately 56 basic block(s).


// Initialize vtable(s) for 55 virtual method(s).
void __nvoc_init_funcTable_GSyncApi(GSyncApi *pThis) {

    // Initialize vtable(s) with 55 per-object function pointer(s).
    __nvoc_init_funcTable_GSyncApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_GSyncApi(GSyncApi *pThis) {
    pThis->__nvoc_pbase_GSyncApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_GSyncApi(pThis);
}

NV_STATUS __nvoc_objCreate_GSyncApi(GSyncApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GSyncApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GSyncApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GSyncApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GSyncApi);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_GSyncApi(pThis);
    status = __nvoc_ctor_GSyncApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GSyncApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GSyncApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GSyncApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GSyncApi(GSyncApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GSyncApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

