#define NVOC_KERNEL_HOSTVGPUDEVICEAPI_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_hostvgpudeviceapi_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xe32156 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceShr;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
void __nvoc_init_funcTable_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
NV_STATUS __nvoc_ctor_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
void __nvoc_init_dataField_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
void __nvoc_dtor_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHostVgpuDeviceShr;

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceShr_KernelHostVgpuDeviceShr = {
    /*pClassDef=*/          &__nvoc_class_def_KernelHostVgpuDeviceShr,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHostVgpuDeviceShr,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceShr_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceShr, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceShr_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceShr, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelHostVgpuDeviceShr = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelHostVgpuDeviceShr_KernelHostVgpuDeviceShr,
        &__nvoc_rtti_KernelHostVgpuDeviceShr_RsShared,
        &__nvoc_rtti_KernelHostVgpuDeviceShr_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceShr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelHostVgpuDeviceShr),
        /*classId=*/            classId(KernelHostVgpuDeviceShr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelHostVgpuDeviceShr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelHostVgpuDeviceShr,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelHostVgpuDeviceShr,
    /*pExportInfo=*/        &__nvoc_export_info_KernelHostVgpuDeviceShr
};

// Down-thunk(s) to bridge KernelHostVgpuDeviceShr methods from ancestors (if any)

// Up-thunk(s) to bridge KernelHostVgpuDeviceShr methods to ancestors (if any)

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHostVgpuDeviceShr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr *pThis) {
    __nvoc_kernelhostvgpudeviceshrDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_KernelHostVgpuDeviceShr_fail_RsShared;
    __nvoc_init_dataField_KernelHostVgpuDeviceShr(pThis);

    status = __nvoc_kernelhostvgpudeviceshrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_KernelHostVgpuDeviceShr_fail__init;
    goto __nvoc_ctor_KernelHostVgpuDeviceShr_exit; // Success

__nvoc_ctor_KernelHostVgpuDeviceShr_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_KernelHostVgpuDeviceShr_fail_RsShared:
__nvoc_ctor_KernelHostVgpuDeviceShr_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelHostVgpuDeviceShr_1(KernelHostVgpuDeviceShr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelHostVgpuDeviceShr_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr *pThis) {
    __nvoc_init_funcTable_KernelHostVgpuDeviceShr_1(pThis);
}

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr *pThis) {
    pThis->__nvoc_pbase_KernelHostVgpuDeviceShr = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
    __nvoc_init_funcTable_KernelHostVgpuDeviceShr(pThis);
}

NV_STATUS __nvoc_objCreate_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelHostVgpuDeviceShr *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelHostVgpuDeviceShr), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelHostVgpuDeviceShr));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelHostVgpuDeviceShr);

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_KernelHostVgpuDeviceShr(pThis);
    status = __nvoc_ctor_KernelHostVgpuDeviceShr(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_KernelHostVgpuDeviceShr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelHostVgpuDeviceShr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelHostVgpuDeviceShr));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelHostVgpuDeviceShr(ppThis, pParent, createFlags);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb12d7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
void __nvoc_init_funcTable_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
NV_STATUS __nvoc_ctor_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
void __nvoc_dtor_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHostVgpuDeviceApi;

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_KernelHostVgpuDeviceApi = {
    /*pClassDef=*/          &__nvoc_class_def_KernelHostVgpuDeviceApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHostVgpuDeviceApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHostVgpuDeviceApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelHostVgpuDeviceApi = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_KernelHostVgpuDeviceApi_KernelHostVgpuDeviceApi,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_Notifier,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_INotifier,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_GpuResource,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_RmResource,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_RmResourceCommon,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_RsResource,
        &__nvoc_rtti_KernelHostVgpuDeviceApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelHostVgpuDeviceApi),
        /*classId=*/            classId(KernelHostVgpuDeviceApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelHostVgpuDeviceApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelHostVgpuDeviceApi,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelHostVgpuDeviceApi,
    /*pExportInfo=*/        &__nvoc_export_info_KernelHostVgpuDeviceApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelHostVgpuDeviceApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840101u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840102u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840103u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840104u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x808u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x808u)
        /*flags=*/      0x808u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840105u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840106u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdEventSetNotification_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0840107u,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdEventSetNotification"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa084010au,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa084010bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdFreeStates_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa084010cu,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdFreeStates"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa084010du,
        /*paramSize=*/  sizeof(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BOOTLOAD_VGPU_TASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kernelhostvgpudeviceapiCtrlCmdSetPlacementId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa084010eu,
        /*paramSize=*/  sizeof(NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_PLACEMENT_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelHostVgpuDeviceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kernelhostvgpudeviceapiCtrlCmdSetPlacementId"
#endif
    },

};

// Down-thunk(s) to bridge KernelHostVgpuDeviceApi methods from ancestors (if any)
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
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NvBool __nvoc_down_thunk_KernelHostVgpuDeviceApi_resCanCopy(struct RsResource *pKernelHostVgpuDeviceApi);    // this

// 1 down-thunk(s) defined to bridge methods in KernelHostVgpuDeviceApi from superclasses

// kernelhostvgpudeviceapiCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_KernelHostVgpuDeviceApi_resCanCopy(struct RsResource *pKernelHostVgpuDeviceApi) {
    return kernelhostvgpudeviceapiCanCopy((struct KernelHostVgpuDeviceApi *)(((unsigned char *) pKernelHostVgpuDeviceApi) - NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// Up-thunk(s) to bridge KernelHostVgpuDeviceApi methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiControl(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiMap(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiUnmap(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiShareCallback(struct KernelHostVgpuDeviceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetRegBaseOffsetAndSize(struct KernelHostVgpuDeviceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetMapAddrSpace(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiInternalControlForward(struct KernelHostVgpuDeviceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetInternalObjectHandle(struct KernelHostVgpuDeviceApi *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiAccessCallback(struct KernelHostVgpuDeviceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemInterMapParams(struct KernelHostVgpuDeviceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiCheckMemInterUnmap(struct KernelHostVgpuDeviceApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemoryMappingDescriptor(struct KernelHostVgpuDeviceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Prologue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Epilogue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Prologue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Epilogue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsDuplicate(struct KernelHostVgpuDeviceApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiPreDestruct(struct KernelHostVgpuDeviceApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiControlFilter(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsPartialUnmapSupported(struct KernelHostVgpuDeviceApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiMapTo(struct KernelHostVgpuDeviceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiUnmapFrom(struct KernelHostVgpuDeviceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiGetRefCount(struct KernelHostVgpuDeviceApi *pResource);    // this
void __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiAddAdditionalDependants(struct RsClient *pClient, struct KernelHostVgpuDeviceApi *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationListPtr(struct KernelHostVgpuDeviceApi *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationShare(struct KernelHostVgpuDeviceApi *pNotifier);    // this
void __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiSetNotificationShare(struct KernelHostVgpuDeviceApi *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiUnregisterEvent(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetOrAllocNotifShare(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// 29 up-thunk(s) defined to bridge methods in KernelHostVgpuDeviceApi to superclasses

// kernelhostvgpudeviceapiControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiControl(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// kernelhostvgpudeviceapiMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiMap(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// kernelhostvgpudeviceapiUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiUnmap(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// kernelhostvgpudeviceapiShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiShareCallback(struct KernelHostVgpuDeviceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// kernelhostvgpudeviceapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetRegBaseOffsetAndSize(struct KernelHostVgpuDeviceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// kernelhostvgpudeviceapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetMapAddrSpace(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// kernelhostvgpudeviceapiInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiInternalControlForward(struct KernelHostVgpuDeviceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)), command, pParams, size);
}

// kernelhostvgpudeviceapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetInternalObjectHandle(struct KernelHostVgpuDeviceApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource)));
}

// kernelhostvgpudeviceapiAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiAccessCallback(struct KernelHostVgpuDeviceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// kernelhostvgpudeviceapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemInterMapParams(struct KernelHostVgpuDeviceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// kernelhostvgpudeviceapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiCheckMemInterUnmap(struct KernelHostVgpuDeviceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// kernelhostvgpudeviceapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemoryMappingDescriptor(struct KernelHostVgpuDeviceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// kernelhostvgpudeviceapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Prologue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kernelhostvgpudeviceapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Epilogue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kernelhostvgpudeviceapiControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Prologue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kernelhostvgpudeviceapiControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Epilogue(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kernelhostvgpudeviceapiIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsDuplicate(struct KernelHostVgpuDeviceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// kernelhostvgpudeviceapiPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiPreDestruct(struct KernelHostVgpuDeviceApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kernelhostvgpudeviceapiControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiControlFilter(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// kernelhostvgpudeviceapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsPartialUnmapSupported(struct KernelHostVgpuDeviceApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kernelhostvgpudeviceapiMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiMapTo(struct KernelHostVgpuDeviceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kernelhostvgpudeviceapiUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiUnmapFrom(struct KernelHostVgpuDeviceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kernelhostvgpudeviceapiGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiGetRefCount(struct KernelHostVgpuDeviceApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kernelhostvgpudeviceapiAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiAddAdditionalDependants(struct RsClient *pClient, struct KernelHostVgpuDeviceApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// kernelhostvgpudeviceapiGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationListPtr(struct KernelHostVgpuDeviceApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier)));
}

// kernelhostvgpudeviceapiGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationShare(struct KernelHostVgpuDeviceApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier)));
}

// kernelhostvgpudeviceapiSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiSetNotificationShare(struct KernelHostVgpuDeviceApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier)), pNotifShare);
}

// kernelhostvgpudeviceapiUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiUnregisterEvent(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// kernelhostvgpudeviceapiGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetOrAllocNotifShare(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHostVgpuDeviceApi = 
{
    /*numEntries=*/     12,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelHostVgpuDeviceApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi *pThis) {
    __nvoc_kernelhostvgpudeviceapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelHostVgpuDeviceApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_KernelHostVgpuDeviceApi_fail_Notifier;
    __nvoc_init_dataField_KernelHostVgpuDeviceApi(pThis);

    status = __nvoc_kernelhostvgpudeviceapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelHostVgpuDeviceApi_fail__init;
    goto __nvoc_ctor_KernelHostVgpuDeviceApi_exit; // Success

__nvoc_ctor_KernelHostVgpuDeviceApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_KernelHostVgpuDeviceApi_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelHostVgpuDeviceApi_fail_GpuResource:
__nvoc_ctor_KernelHostVgpuDeviceApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelHostVgpuDeviceApi_1(KernelHostVgpuDeviceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo -- exported (id=0xa0840101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo__ = &kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState -- exported (id=0xa0840102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState__ = &kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo -- exported (id=0xa0840103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo__ = &kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess -- exported (id=0xa0840104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess__ = &kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf -- exported (id=0xa0840105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x808u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf__ = &kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell -- exported (id=0xa0840106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell__ = &kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdEventSetNotification -- exported (id=0xa0840107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdEventSetNotification__ = &kernelhostvgpudeviceapiCtrlCmdEventSetNotification_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges -- exported (id=0xa084010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges__ = &kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition -- exported (id=0xa084010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition__ = &kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdFreeStates -- exported (id=0xa084010c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdFreeStates__ = &kernelhostvgpudeviceapiCtrlCmdFreeStates_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask -- exported (id=0xa084010d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask__ = &kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_IMPL;
#endif

    // kernelhostvgpudeviceapiCtrlCmdSetPlacementId -- exported (id=0xa084010e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kernelhostvgpudeviceapiCtrlCmdSetPlacementId__ = &kernelhostvgpudeviceapiCtrlCmdSetPlacementId_IMPL;
#endif
} // End __nvoc_init_funcTable_KernelHostVgpuDeviceApi_1 with approximately 12 basic block(s).


// Initialize vtable(s) for 42 virtual method(s).
void __nvoc_init_funcTable_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelHostVgpuDeviceApi vtable = {
        .__kernelhostvgpudeviceapiCanCopy__ = &kernelhostvgpudeviceapiCanCopy_IMPL,    // virtual override (res) base (gpures)
        .GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_KernelHostVgpuDeviceApi_resCanCopy,    // virtual
        .__kernelhostvgpudeviceapiControl__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiControl,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__kernelhostvgpudeviceapiMap__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiMap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__kernelhostvgpudeviceapiUnmap__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiUnmap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__kernelhostvgpudeviceapiShareCallback__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiShareCallback,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__kernelhostvgpudeviceapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
        .__kernelhostvgpudeviceapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__kernelhostvgpudeviceapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__kernelhostvgpudeviceapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__kernelhostvgpudeviceapiAccessCallback__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiAccessCallback,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__kernelhostvgpudeviceapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__kernelhostvgpudeviceapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__kernelhostvgpudeviceapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__kernelhostvgpudeviceapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__kernelhostvgpudeviceapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__kernelhostvgpudeviceapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__kernelhostvgpudeviceapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__kernelhostvgpudeviceapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsDuplicate,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__kernelhostvgpudeviceapiPreDestruct__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiPreDestruct,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__kernelhostvgpudeviceapiControlFilter__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiControlFilter,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__kernelhostvgpudeviceapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__kernelhostvgpudeviceapiMapTo__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiMapTo,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__kernelhostvgpudeviceapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiUnmapFrom,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__kernelhostvgpudeviceapiGetRefCount__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiGetRefCount,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__kernelhostvgpudeviceapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
        .__kernelhostvgpudeviceapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__kernelhostvgpudeviceapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__kernelhostvgpudeviceapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiSetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__kernelhostvgpudeviceapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiUnregisterEvent,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__kernelhostvgpudeviceapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.GpuResource.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.GpuResource.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_vtable = &vtable.GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_vtable = &vtable.Notifier.INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_vtable = &vtable.Notifier;    // (notify) super
    pThis->__nvoc_vtable = &vtable;    // (kernelhostvgpudeviceapi) this

    // Initialize vtable(s) with 12 per-object function pointer(s).
    __nvoc_init_funcTable_KernelHostVgpuDeviceApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi *pThis) {
    pThis->__nvoc_pbase_KernelHostVgpuDeviceApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_KernelHostVgpuDeviceApi(pThis);
}

NV_STATUS __nvoc_objCreate_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelHostVgpuDeviceApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelHostVgpuDeviceApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelHostVgpuDeviceApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelHostVgpuDeviceApi);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_KernelHostVgpuDeviceApi(pThis);
    status = __nvoc_ctor_KernelHostVgpuDeviceApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelHostVgpuDeviceApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelHostVgpuDeviceApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelHostVgpuDeviceApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelHostVgpuDeviceApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

