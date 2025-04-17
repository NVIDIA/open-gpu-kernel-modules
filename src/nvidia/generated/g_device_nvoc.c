#define NVOC_DEVICE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_device_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xe0ac20 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Device;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for Device
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Device(Device*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_Device(Device*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_Device(Device*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_Device(Device*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_Device(Device*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Device;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Device;

// Down-thunk(s) to bridge Device methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_Device_gpuresControl(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_Device_gpuresInternalControlForward(struct GpuResource *pDevice, NvU32 command, void *pParams, NvU32 size);    // this

// Up-thunk(s) to bridge Device methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_deviceMap(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_deviceUnmap(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_deviceShareCallback(struct Device *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_deviceGetRegBaseOffsetAndSize(struct Device *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_deviceGetMapAddrSpace(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NvHandle __nvoc_up_thunk_GpuResource_deviceGetInternalObjectHandle(struct Device *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_deviceAccessCallback(struct Device *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_deviceGetMemInterMapParams(struct Device *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_deviceCheckMemInterUnmap(struct Device *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_deviceGetMemoryMappingDescriptor(struct Device *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_deviceControlSerialization_Prologue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_deviceControlSerialization_Epilogue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_deviceControl_Prologue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_deviceControl_Epilogue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_deviceCanCopy(struct Device *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_deviceIsDuplicate(struct Device *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_devicePreDestruct(struct Device *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_deviceControlFilter(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_deviceIsPartialUnmapSupported(struct Device *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_deviceMapTo(struct Device *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_deviceUnmapFrom(struct Device *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_deviceGetRefCount(struct Device *pResource);    // this
void __nvoc_up_thunk_RsResource_deviceAddAdditionalDependants(struct RsClient *pClient, struct Device *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_Device = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Device),
        /*classId=*/            classId(Device),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Device",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Device,
    /*pCastInfo=*/          &__nvoc_castinfo__Device,
    /*pExportInfo=*/        &__nvoc_export_info__Device
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_Device[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdBifAspmFeatureSupported_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800104u,
        /*paramSize=*/  sizeof(NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdBifAspmFeatureSupported"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdBifAspmCyaUpdate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800105u,
        /*paramSize=*/  sizeof(NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdBifAspmCyaUpdate"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdBifGetPciePowerControlMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800106u,
        /*paramSize=*/  sizeof(NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdBifGetPciePowerControlMask"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetClasslist_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800201u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetClasslist"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetNumSubdevices_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*flags=*/      0x509u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800280u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetNumSubdevices"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuModifyGpuSwStatePersistence_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*flags=*/      0x5u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800287u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuModifyGpuSwStatePersistence"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuQueryGpuSwStatePersistence_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800288u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuQueryGpuSwStatePersistence"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetVirtualizationMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800289u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetVirtualizationMode"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetSparseTextureComputeMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80028cu,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetSparseTextureComputeMode"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuSetSparseTextureComputeMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80028du,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuSetSparseTextureComputeMode"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetVgxCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80028eu,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetVgxCaps"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuVirtualizationSwitchToVga_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800290u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuVirtualizationSwitchToVga"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetSriovCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800291u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetSriovCaps"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetClasslistV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800292u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetClasslistV2"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetFindSubDeviceHandle_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800293u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetFindSubDeviceHandle"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40049u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetBrandCaps_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40049u)
        /*flags=*/      0x40049u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800294u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetBrandCaps"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuSetVgpuVfBar1Size_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800296u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuSetVgpuVfBar1Size"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuSetVgpuHeterogeneousMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800297u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuSetVgpuHeterogeneousMode"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdGpuGetVgpuHeterogeneousMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x800298u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdGpuGetVgpuHeterogeneousMode"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKGrGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*flags=*/      0x10au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801102u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKGrGetCaps"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKGrGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801104u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKGrGetInfo"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKGrGetTpcPartitionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801107u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKGrGetTpcPartitionMode"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKGrSetTpcPartitionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801108u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKGrSetTpcPartitionMode"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKGrGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
        /*flags=*/      0x10au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801109u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GR_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKGrGetCapsV2"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKGrGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801110u,
        /*paramSize=*/  sizeof(NV0080_CTRL_GR_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKGrGetInfoV2"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFbGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801301u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFbGetCaps"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFbGetCompbitStoreInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801306u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFbGetCompbitStoreInfo"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFbGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*flags=*/      0x509u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801307u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FB_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFbGetCapsV2"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801308u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdSetDefaultVidmemPhysicality"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdHostGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801401u,
        /*paramSize=*/  sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdHostGetCaps"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdHostGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
        /*flags=*/      0x518u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801402u,
        /*paramSize=*/  sizeof(NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdHostGetCapsV2"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801701u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoGetCaps"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*flags=*/      0x40148u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801707u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoGetEngineContextProperties"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoGetChannelList_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80170du,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoGetChannelList"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40049u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40049u)
        /*flags=*/      0x40049u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80170eu,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoGetLatencyBufferSize"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoSetChannelProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80170fu,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoSetChannelProperties"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoStopRunlist_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*flags=*/      0x244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801711u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoStopRunlist"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoStartRunlist_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*flags=*/      0x244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801712u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_START_RUNLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoStartRunlist"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801713u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoGetCapsV2"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFifoIdleChannels_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801714u,
        /*paramSize=*/  sizeof(NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFifoIdleChannels"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaGetPteInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801801u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaGetPteInfo"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaFlush_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801805u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_FLUSH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaFlush"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801806u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaAdvSchedGetVaCaps"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaGetPdeInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801809u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaGetPdeInfo"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaSetPteInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80180au,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaSetPteInfo"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaInvalidateTLB_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80180cu,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaInvalidateTLB"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80180du,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaGetCaps"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaSetVASpaceSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80180eu,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaSetVASpaceSize"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaUpdatePde2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80180fu,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaUpdatePde2"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaEnablePrivilegedRange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801810u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaEnablePrivilegedRange"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaSetDefaultVASpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c000u)
        /*flags=*/      0x1c000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801812u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaSetDefaultVASpace"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaSetPageDirectory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*flags=*/      0x14004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801813u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaSetPageDirectory"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdDmaUnsetPageDirectory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*flags=*/      0x14004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801814u,
        /*paramSize=*/  sizeof(NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdDmaUnsetPageDirectory"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdKPerfCudaLimitSetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801909u,
        /*paramSize=*/  sizeof(NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdKPerfCudaLimitSetControl"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdMsencGetCapsV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*flags=*/      0x40148u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801b02u,
        /*paramSize=*/  sizeof(NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdMsencGetCapsV2"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdBspGetCapsV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40148u)
        /*flags=*/      0x40148u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801c02u,
        /*paramSize=*/  sizeof(NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdBspGetCapsV2"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdOsUnixVTSwitch_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*flags=*/      0x1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801e01u,
        /*paramSize=*/  sizeof(NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdOsUnixVTSwitch"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdOsUnixVTGetFBInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*flags=*/      0x1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801e02u,
        /*paramSize=*/  sizeof(NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdOsUnixVTGetFBInfo"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdNvjpgGetCapsV2_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x801f02u,
        /*paramSize=*/  sizeof(NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdNvjpgGetCapsV2"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdInternalPerfCudaLimitDisable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x802004u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdInternalPerfCudaLimitDisable"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdInternalPerfCudaLimitSetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
        /*flags=*/      0x1d8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x802009u,
        /*paramSize=*/  sizeof(NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdInternalPerfCudaLimitSetControl"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) deviceCtrlCmdFbSetZbcReferenced_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x80200au,
        /*paramSize=*/  sizeof(NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Device.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "deviceCtrlCmdFbSetZbcReferenced"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__Device __nvoc_metadata__Device = {
    .rtti.pClassDef = &__nvoc_class_def_Device,    // (device) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Device,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(Device, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__deviceControl__ = &deviceControl_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &__nvoc_down_thunk_Device_gpuresControl,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__deviceInternalControlForward__ = &deviceInternalControlForward_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &__nvoc_down_thunk_Device_gpuresInternalControlForward,    // virtual
    .vtable.__deviceMap__ = &__nvoc_up_thunk_GpuResource_deviceMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__deviceUnmap__ = &__nvoc_up_thunk_GpuResource_deviceUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__deviceShareCallback__ = &__nvoc_up_thunk_GpuResource_deviceShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__deviceGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_deviceGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__deviceGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_deviceGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__deviceGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_deviceGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__deviceAccessCallback__ = &__nvoc_up_thunk_RmResource_deviceAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__deviceGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_deviceGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__deviceCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_deviceCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__deviceGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_deviceGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__deviceControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_deviceControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__deviceControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_deviceControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__deviceControl_Prologue__ = &__nvoc_up_thunk_RmResource_deviceControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__deviceControl_Epilogue__ = &__nvoc_up_thunk_RmResource_deviceControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__deviceCanCopy__ = &__nvoc_up_thunk_RsResource_deviceCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__deviceIsDuplicate__ = &__nvoc_up_thunk_RsResource_deviceIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__devicePreDestruct__ = &__nvoc_up_thunk_RsResource_devicePreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__deviceControlFilter__ = &__nvoc_up_thunk_RsResource_deviceControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__deviceIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_deviceIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__deviceMapTo__ = &__nvoc_up_thunk_RsResource_deviceMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__deviceUnmapFrom__ = &__nvoc_up_thunk_RsResource_deviceUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__deviceGetRefCount__ = &__nvoc_up_thunk_RsResource_deviceGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__deviceAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_deviceAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Device = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__Device.rtti,    // [0]: (device) this
        &__nvoc_metadata__Device.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 2 down-thunk(s) defined to bridge methods in Device from superclasses

// deviceControl: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_Device_gpuresControl(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return deviceControl((struct Device *)(((unsigned char *) pResource) - NV_OFFSETOF(Device, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// deviceInternalControlForward: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_Device_gpuresInternalControlForward(struct GpuResource *pDevice, NvU32 command, void *pParams, NvU32 size) {
    return deviceInternalControlForward((struct Device *)(((unsigned char *) pDevice) - NV_OFFSETOF(Device, __nvoc_base_GpuResource)), command, pParams, size);
}


// 23 up-thunk(s) defined to bridge methods in Device to superclasses

// deviceMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_deviceMap(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// deviceUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_deviceUnmap(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// deviceShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_deviceShareCallback(struct Device *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// deviceGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_deviceGetRegBaseOffsetAndSize(struct Device *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// deviceGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_deviceGetMapAddrSpace(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// deviceGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_deviceGetInternalObjectHandle(struct Device *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource)));
}

// deviceAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_deviceAccessCallback(struct Device *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// deviceGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_deviceGetMemInterMapParams(struct Device *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// deviceCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_deviceCheckMemInterUnmap(struct Device *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// deviceGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_deviceGetMemoryMappingDescriptor(struct Device *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// deviceControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_deviceControlSerialization_Prologue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// deviceControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_deviceControlSerialization_Epilogue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// deviceControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_deviceControl_Prologue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// deviceControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_deviceControl_Epilogue(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// deviceCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_deviceCanCopy(struct Device *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// deviceIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_deviceIsDuplicate(struct Device *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// devicePreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_devicePreDestruct(struct Device *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// deviceControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_deviceControlFilter(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// deviceIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_deviceIsPartialUnmapSupported(struct Device *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// deviceMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_deviceMapTo(struct Device *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// deviceUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_deviceUnmapFrom(struct Device *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// deviceGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_deviceGetRefCount(struct Device *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// deviceAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_deviceAddAdditionalDependants(struct RsClient *pClient, struct Device *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Device, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__Device = 
{
    /*numEntries=*/     62,
    /*pExportEntries=*/ __nvoc_exported_method_def_Device
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Device(Device *pThis) {
    __nvoc_deviceDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Device(Device *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Device(Device *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Device_fail_GpuResource;
    __nvoc_init_dataField_Device(pThis, pRmhalspecowner);

    status = __nvoc_deviceConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Device_fail__init;
    goto __nvoc_ctor_Device_exit; // Success

__nvoc_ctor_Device_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_Device_fail_GpuResource:
__nvoc_ctor_Device_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Device_1(Device *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // deviceCtrlCmdBifAspmFeatureSupported -- exported (id=0x800104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__deviceCtrlCmdBifAspmFeatureSupported__ = &deviceCtrlCmdBifAspmFeatureSupported_IMPL;
#endif

    // deviceCtrlCmdBifAspmCyaUpdate -- exported (id=0x800105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__deviceCtrlCmdBifAspmCyaUpdate__ = &deviceCtrlCmdBifAspmCyaUpdate_IMPL;
#endif

    // deviceCtrlCmdBifGetPciePowerControlMask -- exported (id=0x800106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__deviceCtrlCmdBifGetPciePowerControlMask__ = &deviceCtrlCmdBifGetPciePowerControlMask_IMPL;
#endif

    // deviceCtrlCmdDmaGetPteInfo -- exported (id=0x801801)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaGetPteInfo__ = &deviceCtrlCmdDmaGetPteInfo_IMPL;
#endif

    // deviceCtrlCmdDmaUpdatePde2 -- exported (id=0x80180f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaUpdatePde2__ = &deviceCtrlCmdDmaUpdatePde2_IMPL;
#endif

    // deviceCtrlCmdDmaSetPageDirectory -- exported (id=0x801813)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
    pThis->__deviceCtrlCmdDmaSetPageDirectory__ = &deviceCtrlCmdDmaSetPageDirectory_IMPL;
#endif

    // deviceCtrlCmdDmaUnsetPageDirectory -- exported (id=0x801814)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
    pThis->__deviceCtrlCmdDmaUnsetPageDirectory__ = &deviceCtrlCmdDmaUnsetPageDirectory_IMPL;
#endif

    // deviceCtrlCmdDmaFlush -- halified (2 hals) exported (id=0x801805) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdDmaFlush__ = &deviceCtrlCmdDmaFlush_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdDmaFlush__ = &deviceCtrlCmdDmaFlush_5baef9;
    }

    // deviceCtrlCmdDmaAdvSchedGetVaCaps -- exported (id=0x801806)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__deviceCtrlCmdDmaAdvSchedGetVaCaps__ = &deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL;
#endif

    // deviceCtrlCmdDmaGetPdeInfo -- exported (id=0x801809)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaGetPdeInfo__ = &deviceCtrlCmdDmaGetPdeInfo_IMPL;
#endif

    // deviceCtrlCmdDmaSetPteInfo -- exported (id=0x80180a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaSetPteInfo__ = &deviceCtrlCmdDmaSetPteInfo_IMPL;
#endif

    // deviceCtrlCmdDmaInvalidateTLB -- exported (id=0x80180c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaInvalidateTLB__ = &deviceCtrlCmdDmaInvalidateTLB_IMPL;
#endif

    // deviceCtrlCmdDmaGetCaps -- exported (id=0x80180d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__deviceCtrlCmdDmaGetCaps__ = &deviceCtrlCmdDmaGetCaps_IMPL;
#endif

    // deviceCtrlCmdDmaSetVASpaceSize -- exported (id=0x80180e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaSetVASpaceSize__ = &deviceCtrlCmdDmaSetVASpaceSize_IMPL;
#endif

    // deviceCtrlCmdDmaEnablePrivilegedRange -- exported (id=0x801810)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdDmaEnablePrivilegedRange__ = &deviceCtrlCmdDmaEnablePrivilegedRange_IMPL;
#endif

    // deviceCtrlCmdDmaSetDefaultVASpace -- exported (id=0x801812)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1c000u)
    pThis->__deviceCtrlCmdDmaSetDefaultVASpace__ = &deviceCtrlCmdDmaSetDefaultVASpace_IMPL;
#endif

    // deviceCtrlCmdKGrGetCaps -- exported (id=0x801102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
    pThis->__deviceCtrlCmdKGrGetCaps__ = &deviceCtrlCmdKGrGetCaps_IMPL;
#endif

    // deviceCtrlCmdKGrGetCapsV2 -- exported (id=0x801109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10au)
    pThis->__deviceCtrlCmdKGrGetCapsV2__ = &deviceCtrlCmdKGrGetCapsV2_IMPL;
#endif

    // deviceCtrlCmdKGrGetInfo -- exported (id=0x801104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__deviceCtrlCmdKGrGetInfo__ = &deviceCtrlCmdKGrGetInfo_IMPL;
#endif

    // deviceCtrlCmdKGrGetInfoV2 -- exported (id=0x801110)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdKGrGetInfoV2__ = &deviceCtrlCmdKGrGetInfoV2_IMPL;
#endif

    // deviceCtrlCmdKGrGetTpcPartitionMode -- exported (id=0x801107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__deviceCtrlCmdKGrGetTpcPartitionMode__ = &deviceCtrlCmdKGrGetTpcPartitionMode_IMPL;
#endif

    // deviceCtrlCmdKGrSetTpcPartitionMode -- exported (id=0x801108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__deviceCtrlCmdKGrSetTpcPartitionMode__ = &deviceCtrlCmdKGrSetTpcPartitionMode_IMPL;
#endif

    // deviceCtrlCmdFbGetCompbitStoreInfo -- exported (id=0x801306)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__deviceCtrlCmdFbGetCompbitStoreInfo__ = &deviceCtrlCmdFbGetCompbitStoreInfo_IMPL;
#endif

    // deviceCtrlCmdFbSetZbcReferenced -- exported (id=0x80200a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdFbSetZbcReferenced__ = &deviceCtrlCmdFbSetZbcReferenced_IMPL;
#endif

    // deviceCtrlCmdFbGetCaps -- exported (id=0x801301)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__deviceCtrlCmdFbGetCaps__ = &deviceCtrlCmdFbGetCaps_IMPL;
#endif

    // deviceCtrlCmdFbGetCapsV2 -- exported (id=0x801307)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
    pThis->__deviceCtrlCmdFbGetCapsV2__ = &deviceCtrlCmdFbGetCapsV2_IMPL;
#endif

    // deviceCtrlCmdSetDefaultVidmemPhysicality -- exported (id=0x801308)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__deviceCtrlCmdSetDefaultVidmemPhysicality__ = &deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL;
#endif

    // deviceCtrlCmdFifoGetCaps -- exported (id=0x801701)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__deviceCtrlCmdFifoGetCaps__ = &deviceCtrlCmdFifoGetCaps_IMPL;
#endif

    // deviceCtrlCmdFifoGetCapsV2 -- exported (id=0x801713)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__deviceCtrlCmdFifoGetCapsV2__ = &deviceCtrlCmdFifoGetCapsV2_IMPL;
#endif

    // deviceCtrlCmdFifoGetEngineContextProperties -- halified (2 hals) exported (id=0x801707) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdFifoGetEngineContextProperties__ = &deviceCtrlCmdFifoGetEngineContextProperties_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdFifoGetEngineContextProperties__ = &deviceCtrlCmdFifoGetEngineContextProperties_92bfc3;
    }

    // deviceCtrlCmdFifoStopRunlist -- exported (id=0x801711)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
    pThis->__deviceCtrlCmdFifoStopRunlist__ = &deviceCtrlCmdFifoStopRunlist_IMPL;
#endif

    // deviceCtrlCmdFifoStartRunlist -- exported (id=0x801712)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
    pThis->__deviceCtrlCmdFifoStartRunlist__ = &deviceCtrlCmdFifoStartRunlist_IMPL;
#endif

    // deviceCtrlCmdFifoGetChannelList -- exported (id=0x80170d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__deviceCtrlCmdFifoGetChannelList__ = &deviceCtrlCmdFifoGetChannelList_IMPL;
#endif

    // deviceCtrlCmdFifoGetLatencyBufferSize -- halified (2 hals) exported (id=0x80170e) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdFifoGetLatencyBufferSize__ = &deviceCtrlCmdFifoGetLatencyBufferSize_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdFifoGetLatencyBufferSize__ = &deviceCtrlCmdFifoGetLatencyBufferSize_92bfc3;
    }

    // deviceCtrlCmdFifoSetChannelProperties -- exported (id=0x80170f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__deviceCtrlCmdFifoSetChannelProperties__ = &deviceCtrlCmdFifoSetChannelProperties_IMPL;
#endif

    // deviceCtrlCmdFifoIdleChannels -- halified (2 hals) exported (id=0x801714) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdFifoIdleChannels__ = &deviceCtrlCmdFifoIdleChannels_46f6a7;
    }
    else
    {
        pThis->__deviceCtrlCmdFifoIdleChannels__ = &deviceCtrlCmdFifoIdleChannels_IMPL;
    }

    // deviceCtrlCmdHostGetCaps -- exported (id=0x801401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__deviceCtrlCmdHostGetCaps__ = &deviceCtrlCmdHostGetCaps_IMPL;
#endif

    // deviceCtrlCmdHostGetCapsV2 -- exported (id=0x801402)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x518u)
    pThis->__deviceCtrlCmdHostGetCapsV2__ = &deviceCtrlCmdHostGetCapsV2_IMPL;
#endif

    // deviceCtrlCmdKPerfCudaLimitSetControl -- exported (id=0x801909)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__deviceCtrlCmdKPerfCudaLimitSetControl__ = &deviceCtrlCmdKPerfCudaLimitSetControl_IMPL;
#endif

    // deviceCtrlCmdInternalPerfCudaLimitSetControl -- exported (id=0x802009)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1d8u)
    pThis->__deviceCtrlCmdInternalPerfCudaLimitSetControl__ = &deviceCtrlCmdInternalPerfCudaLimitSetControl_IMPL;
#endif

    // deviceCtrlCmdInternalPerfCudaLimitDisable -- exported (id=0x802004)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__deviceCtrlCmdInternalPerfCudaLimitDisable__ = &deviceCtrlCmdInternalPerfCudaLimitDisable_IMPL;
#endif

    // deviceCtrlCmdGpuGetClasslist -- exported (id=0x800201)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__deviceCtrlCmdGpuGetClasslist__ = &deviceCtrlCmdGpuGetClasslist_IMPL;
#endif

    // deviceCtrlCmdGpuGetClasslistV2 -- exported (id=0x800292)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__deviceCtrlCmdGpuGetClasslistV2__ = &deviceCtrlCmdGpuGetClasslistV2_IMPL;
#endif

    // deviceCtrlCmdGpuGetNumSubdevices -- exported (id=0x800280)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
    pThis->__deviceCtrlCmdGpuGetNumSubdevices__ = &deviceCtrlCmdGpuGetNumSubdevices_IMPL;
#endif

    // deviceCtrlCmdGpuModifyGpuSwStatePersistence -- exported (id=0x800287)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
    pThis->__deviceCtrlCmdGpuModifyGpuSwStatePersistence__ = &deviceCtrlCmdGpuModifyGpuSwStatePersistence_IMPL;
#endif

    // deviceCtrlCmdGpuQueryGpuSwStatePersistence -- exported (id=0x800288)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__deviceCtrlCmdGpuQueryGpuSwStatePersistence__ = &deviceCtrlCmdGpuQueryGpuSwStatePersistence_IMPL;
#endif

    // deviceCtrlCmdGpuGetVirtualizationMode -- exported (id=0x800289)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__deviceCtrlCmdGpuGetVirtualizationMode__ = &deviceCtrlCmdGpuGetVirtualizationMode_IMPL;
#endif

    // deviceCtrlCmdGpuSetVgpuVfBar1Size -- exported (id=0x800296)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__deviceCtrlCmdGpuSetVgpuVfBar1Size__ = &deviceCtrlCmdGpuSetVgpuVfBar1Size_IMPL;
#endif

    // deviceCtrlCmdGpuGetSparseTextureComputeMode -- exported (id=0x80028c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__deviceCtrlCmdGpuGetSparseTextureComputeMode__ = &deviceCtrlCmdGpuGetSparseTextureComputeMode_IMPL;
#endif

    // deviceCtrlCmdGpuSetSparseTextureComputeMode -- exported (id=0x80028d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__deviceCtrlCmdGpuSetSparseTextureComputeMode__ = &deviceCtrlCmdGpuSetSparseTextureComputeMode_IMPL;
#endif

    // deviceCtrlCmdGpuGetVgxCaps -- exported (id=0x80028e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__deviceCtrlCmdGpuGetVgxCaps__ = &deviceCtrlCmdGpuGetVgxCaps_IMPL;
#endif

    // deviceCtrlCmdGpuGetBrandCaps -- halified (2 hals) exported (id=0x800294) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdGpuGetBrandCaps__ = &deviceCtrlCmdGpuGetBrandCaps_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdGpuGetBrandCaps__ = &deviceCtrlCmdGpuGetBrandCaps_5baef9;
    }

    // deviceCtrlCmdGpuVirtualizationSwitchToVga -- exported (id=0x800290)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdGpuVirtualizationSwitchToVga__ = &deviceCtrlCmdGpuVirtualizationSwitchToVga_IMPL;
#endif

    // deviceCtrlCmdGpuSetVgpuHeterogeneousMode -- exported (id=0x800297)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdGpuSetVgpuHeterogeneousMode__ = &deviceCtrlCmdGpuSetVgpuHeterogeneousMode_IMPL;
#endif

    // deviceCtrlCmdGpuGetVgpuHeterogeneousMode -- exported (id=0x800298)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdGpuGetVgpuHeterogeneousMode__ = &deviceCtrlCmdGpuGetVgpuHeterogeneousMode_IMPL;
#endif

    // deviceCtrlCmdGpuGetSriovCaps -- exported (id=0x800291)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__deviceCtrlCmdGpuGetSriovCaps__ = &deviceCtrlCmdGpuGetSriovCaps_IMPL;
#endif

    // deviceCtrlCmdGpuGetFindSubDeviceHandle -- exported (id=0x800293)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__deviceCtrlCmdGpuGetFindSubDeviceHandle__ = &deviceCtrlCmdGpuGetFindSubDeviceHandle_IMPL;
#endif

    // deviceCtrlCmdMsencGetCapsV2 -- halified (2 hals) exported (id=0x801b02) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdMsencGetCapsV2__ = &deviceCtrlCmdMsencGetCapsV2_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdMsencGetCapsV2__ = &deviceCtrlCmdMsencGetCapsV2_5baef9;
    }

    // deviceCtrlCmdBspGetCapsV2 -- halified (2 hals) exported (id=0x801c02) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdBspGetCapsV2__ = &deviceCtrlCmdBspGetCapsV2_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdBspGetCapsV2__ = &deviceCtrlCmdBspGetCapsV2_5baef9;
    }

    // deviceCtrlCmdNvjpgGetCapsV2 -- halified (2 hals) exported (id=0x801f02) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__deviceCtrlCmdNvjpgGetCapsV2__ = &deviceCtrlCmdNvjpgGetCapsV2_VF;
    }
    else
    {
        pThis->__deviceCtrlCmdNvjpgGetCapsV2__ = &deviceCtrlCmdNvjpgGetCapsV2_c04480;
    }

    // deviceCtrlCmdOsUnixVTSwitch -- exported (id=0x801e01)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
    pThis->__deviceCtrlCmdOsUnixVTSwitch__ = &deviceCtrlCmdOsUnixVTSwitch_IMPL;
#endif

    // deviceCtrlCmdOsUnixVTGetFBInfo -- exported (id=0x801e02)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
    pThis->__deviceCtrlCmdOsUnixVTGetFBInfo__ = &deviceCtrlCmdOsUnixVTGetFBInfo_IMPL;
#endif
} // End __nvoc_init_funcTable_Device_1 with approximately 70 basic block(s).


// Initialize vtable(s) for 87 virtual method(s).
void __nvoc_init_funcTable_Device(Device *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 62 per-object function pointer(s).
    __nvoc_init_funcTable_Device_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__Device(Device *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_Device = pThis;    // (device) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__Device.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__Device.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Device;    // (device) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Device(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Device(Device **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Device *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Device), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Device));

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

    __nvoc_init__Device(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Device(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Device_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Device_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Device));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Device(Device **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Device(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

