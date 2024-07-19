#define NVOC_SUBDEVICE_DIAG_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_subdevice_diag_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xaa3066 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_DiagApi(DiagApi*, RmHalspecOwner* );
void __nvoc_init_funcTable_DiagApi(DiagApi*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_DiagApi(DiagApi*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DiagApi(DiagApi*, RmHalspecOwner* );
void __nvoc_dtor_DiagApi(DiagApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DiagApi;

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_DiagApi = {
    /*pClassDef=*/          &__nvoc_class_def_DiagApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DiagApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DiagApi = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_DiagApi_DiagApi,
        &__nvoc_rtti_DiagApi_Notifier,
        &__nvoc_rtti_DiagApi_INotifier,
        &__nvoc_rtti_DiagApi_GpuResource,
        &__nvoc_rtti_DiagApi_RmResource,
        &__nvoc_rtti_DiagApi_RmResourceCommon,
        &__nvoc_rtti_DiagApi_RsResource,
        &__nvoc_rtti_DiagApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DiagApi),
        /*classId=*/            classId(DiagApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DiagApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DiagApi,
    /*pCastInfo=*/          &__nvoc_castinfo_DiagApi,
    /*pExportInfo=*/        &__nvoc_export_info_DiagApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DiagApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoCheckEngineContext_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0401u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoCheckEngineContext"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoEnableVirtualContext_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0402u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoEnableVirtualContext"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoGetChannelState_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0403u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoGetChannelState"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbCtrlGpuCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0506u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbCtrlGpuCache"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbEccSetKillPtr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f050eu,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbEccSetKillPtr"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbClearRemappedRows_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0515u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbClearRemappedRows"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifPBIWriteCommand_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0701u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifPBIWriteCommand"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifConfigRegRead_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0702u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifConfigRegRead"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifConfigRegWrite_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0703u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifConfigRegWrite"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0704u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifInfo"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuGetRamSvopValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1101u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuGetRamSvopValues"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuSetRamSvopValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1102u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuSetRamSvopValues"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuVerifyInforom_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1105u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuVerifyInforom"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdUcodeCoverageGetState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1901u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeCoverageGetState"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdUcodeCoverageSetState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1902u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeCoverageSetState"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdUcodeCoverageGetData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1903u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeCoverageGetData"
#endif
    },

};

// 2 down-thunk(s) defined to bridge methods in DiagApi from superclasses

// diagapiControl: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_DiagApi_gpuresControl(struct GpuResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControl((struct DiagApi *)(((unsigned char *) pDiagApi) - __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, pParams);
}

// diagapiControlFilter: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_DiagApi_resControlFilter(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControlFilter((struct DiagApi *)(((unsigned char *) pDiagApi) - __nvoc_rtti_DiagApi_RsResource.offset), pCallContext, pParams);
}


// 28 up-thunk(s) defined to bridge methods in DiagApi to superclasses

// diagapiMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_diagapiMap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// diagapiUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_diagapiUnmap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, pCpuMapping);
}

// diagapiShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_diagapiShareCallback(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// diagapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pGpu, pOffset, pSize);
}

// diagapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_diagapiGetMapAddrSpace(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// diagapiInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_diagapiInternalControlForward(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), command, pParams, size);
}

// diagapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_diagapiGetInternalObjectHandle(struct DiagApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset));
}

// diagapiAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_diagapiAccessCallback(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// diagapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_diagapiGetMemInterMapParams(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DiagApi_RmResource.offset), pParams);
}

// diagapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_diagapiCheckMemInterUnmap(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DiagApi_RmResource.offset), bSubdeviceHandleProvided);
}

// diagapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_diagapiGetMemoryMappingDescriptor(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DiagApi_RmResource.offset), ppMemDesc);
}

// diagapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_diagapiControlSerialization_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

// diagapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_diagapiControlSerialization_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

// diagapiControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_diagapiControl_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

// diagapiControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_diagapiControl_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

// diagapiCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_diagapiCanCopy(struct DiagApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

// diagapiIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_diagapiIsDuplicate(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset), hMemory, pDuplicate);
}

// diagapiPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_diagapiPreDestruct(struct DiagApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

// diagapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_diagapiIsPartialUnmapSupported(struct DiagApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

// diagapiMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_diagapiMapTo(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pParams);
}

// diagapiUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_diagapiUnmapFrom(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pParams);
}

// diagapiGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_diagapiGetRefCount(struct DiagApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

// diagapiAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_diagapiAddAdditionalDependants(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pReference);
}

// diagapiGetNotificationListPtr: virtual inherited (notify) base (notify)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_diagapiGetNotificationListPtr(struct DiagApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset));
}

// diagapiGetNotificationShare: virtual inherited (notify) base (notify)
static struct NotifShare * __nvoc_up_thunk_Notifier_diagapiGetNotificationShare(struct DiagApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset));
}

// diagapiSetNotificationShare: virtual inherited (notify) base (notify)
static void __nvoc_up_thunk_Notifier_diagapiSetNotificationShare(struct DiagApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset), pNotifShare);
}

// diagapiUnregisterEvent: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_diagapiUnregisterEvent(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// diagapiGetOrAllocNotifShare: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_diagapiGetOrAllocNotifShare(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_DiagApi = 
{
    /*numEntries=*/     16,
    /*pExportEntries=*/ __nvoc_exported_method_def_DiagApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DiagApi(DiagApi *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_Notifier;
    __nvoc_init_dataField_DiagApi(pThis, pRmhalspecowner);

    status = __nvoc_diagapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail__init;
    goto __nvoc_ctor_DiagApi_exit; // Success

__nvoc_ctor_DiagApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_DiagApi_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DiagApi_fail_GpuResource:
__nvoc_ctor_DiagApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DiagApi_1(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // diagapiControl -- virtual override (res) base (gpures)
    pThis->__diagapiControl__ = &diagapiControl_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresControl__ = &__nvoc_down_thunk_DiagApi_gpuresControl;

    // diagapiControlFilter -- virtual override (res) base (gpures)
    pThis->__diagapiControlFilter__ = &diagapiControlFilter_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__ = &__nvoc_down_thunk_DiagApi_resControlFilter;

    // diagapiCtrlCmdFifoCheckEngineContext -- exported (id=0x208f0401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdFifoCheckEngineContext__ = &diagapiCtrlCmdFifoCheckEngineContext_IMPL;
#endif

    // diagapiCtrlCmdFifoEnableVirtualContext -- exported (id=0x208f0402)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__diagapiCtrlCmdFifoEnableVirtualContext__ = &diagapiCtrlCmdFifoEnableVirtualContext_IMPL;
#endif

    // diagapiCtrlCmdFifoGetChannelState -- halified (2 hals) exported (id=0x208f0403) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_46f6a7;
    }
    else
    {
        pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_IMPL;
    }

    // diagapiCtrlCmdFbCtrlGpuCache -- exported (id=0x208f0506)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdFbCtrlGpuCache__ = &diagapiCtrlCmdFbCtrlGpuCache_IMPL;
#endif

    // diagapiCtrlCmdFbEccSetKillPtr -- exported (id=0x208f050e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdFbEccSetKillPtr__ = &diagapiCtrlCmdFbEccSetKillPtr_IMPL;
#endif

    // diagapiCtrlCmdFbClearRemappedRows -- exported (id=0x208f0515)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__diagapiCtrlCmdFbClearRemappedRows__ = &diagapiCtrlCmdFbClearRemappedRows_IMPL;
#endif

    // diagapiCtrlCmdGpuGetRamSvopValues -- exported (id=0x208f1101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdGpuGetRamSvopValues__ = &diagapiCtrlCmdGpuGetRamSvopValues_IMPL;
#endif

    // diagapiCtrlCmdGpuSetRamSvopValues -- exported (id=0x208f1102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdGpuSetRamSvopValues__ = &diagapiCtrlCmdGpuSetRamSvopValues_IMPL;
#endif

    // diagapiCtrlCmdGpuVerifyInforom -- exported (id=0x208f1105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdGpuVerifyInforom__ = &diagapiCtrlCmdGpuVerifyInforom_IMPL;
#endif

    // diagapiCtrlCmdBifPBIWriteCommand -- exported (id=0x208f0701)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifPBIWriteCommand__ = &diagapiCtrlCmdBifPBIWriteCommand_IMPL;
#endif

    // diagapiCtrlCmdBifConfigRegRead -- exported (id=0x208f0702)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifConfigRegRead__ = &diagapiCtrlCmdBifConfigRegRead_IMPL;
#endif

    // diagapiCtrlCmdBifConfigRegWrite -- exported (id=0x208f0703)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifConfigRegWrite__ = &diagapiCtrlCmdBifConfigRegWrite_IMPL;
#endif

    // diagapiCtrlCmdBifInfo -- exported (id=0x208f0704)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifInfo__ = &diagapiCtrlCmdBifInfo_IMPL;
#endif

    // diagapiCtrlCmdUcodeCoverageGetState -- exported (id=0x208f1901)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdUcodeCoverageGetState__ = &diagapiCtrlCmdUcodeCoverageGetState_IMPL;
#endif

    // diagapiCtrlCmdUcodeCoverageSetState -- exported (id=0x208f1902)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdUcodeCoverageSetState__ = &diagapiCtrlCmdUcodeCoverageSetState_IMPL;
#endif

    // diagapiCtrlCmdUcodeCoverageGetData -- exported (id=0x208f1903)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdUcodeCoverageGetData__ = &diagapiCtrlCmdUcodeCoverageGetData_IMPL;
#endif

    // diagapiMap -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiMap__ = &__nvoc_up_thunk_GpuResource_diagapiMap;

    // diagapiUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiUnmap__ = &__nvoc_up_thunk_GpuResource_diagapiUnmap;

    // diagapiShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiShareCallback__ = &__nvoc_up_thunk_GpuResource_diagapiShareCallback;

    // diagapiGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize;

    // diagapiGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_diagapiGetMapAddrSpace;

    // diagapiInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_diagapiInternalControlForward;

    // diagapiGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__diagapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_diagapiGetInternalObjectHandle;

    // diagapiAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiAccessCallback__ = &__nvoc_up_thunk_RmResource_diagapiAccessCallback;

    // diagapiGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_diagapiGetMemInterMapParams;

    // diagapiCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_diagapiCheckMemInterUnmap;

    // diagapiGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_diagapiGetMemoryMappingDescriptor;

    // diagapiControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_diagapiControlSerialization_Prologue;

    // diagapiControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_diagapiControlSerialization_Epilogue;

    // diagapiControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_diagapiControl_Prologue;

    // diagapiControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__diagapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_diagapiControl_Epilogue;

    // diagapiCanCopy -- virtual inherited (res) base (gpures)
    pThis->__diagapiCanCopy__ = &__nvoc_up_thunk_RsResource_diagapiCanCopy;

    // diagapiIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__diagapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_diagapiIsDuplicate;

    // diagapiPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__diagapiPreDestruct__ = &__nvoc_up_thunk_RsResource_diagapiPreDestruct;

    // diagapiIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__diagapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_diagapiIsPartialUnmapSupported;

    // diagapiMapTo -- virtual inherited (res) base (gpures)
    pThis->__diagapiMapTo__ = &__nvoc_up_thunk_RsResource_diagapiMapTo;

    // diagapiUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__diagapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_diagapiUnmapFrom;

    // diagapiGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__diagapiGetRefCount__ = &__nvoc_up_thunk_RsResource_diagapiGetRefCount;

    // diagapiAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__diagapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_diagapiAddAdditionalDependants;

    // diagapiGetNotificationListPtr -- virtual inherited (notify) base (notify)
    pThis->__diagapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_diagapiGetNotificationListPtr;

    // diagapiGetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__diagapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_diagapiGetNotificationShare;

    // diagapiSetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__diagapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_diagapiSetNotificationShare;

    // diagapiUnregisterEvent -- virtual inherited (notify) base (notify)
    pThis->__diagapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_diagapiUnregisterEvent;

    // diagapiGetOrAllocNotifShare -- virtual inherited (notify) base (notify)
    pThis->__diagapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_diagapiGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_DiagApi_1 with approximately 49 basic block(s).


// Initialize vtable(s) for 46 virtual method(s).
void __nvoc_init_funcTable_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 46 per-object function pointer(s).
    __nvoc_init_funcTable_DiagApi_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DiagApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_DiagApi(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DiagApi(DiagApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DiagApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DiagApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DiagApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DiagApi);

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

    __nvoc_init_DiagApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DiagApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DiagApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DiagApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DiagApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DiagApi(DiagApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DiagApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

