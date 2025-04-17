#define NVOC_KERNEL_HOSTVGPUDEVICEAPI_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_hostvgpudeviceapi_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xe32156 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceShr;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for KernelHostVgpuDeviceShr
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
void __nvoc_init_funcTable_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
NV_STATUS __nvoc_ctor_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
void __nvoc_init_dataField_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);
void __nvoc_dtor_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelHostVgpuDeviceShr;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHostVgpuDeviceShr;

// Down-thunk(s) to bridge KernelHostVgpuDeviceShr methods from ancestors (if any)

// Up-thunk(s) to bridge KernelHostVgpuDeviceShr methods to ancestors (if any)

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelHostVgpuDeviceShr,
    /*pExportInfo=*/        &__nvoc_export_info__KernelHostVgpuDeviceShr
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelHostVgpuDeviceShr __nvoc_metadata__KernelHostVgpuDeviceShr = {
    .rtti.pClassDef = &__nvoc_class_def_KernelHostVgpuDeviceShr,    // (kernelhostvgpudeviceshr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHostVgpuDeviceShr,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceShr, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceShr, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelHostVgpuDeviceShr = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelHostVgpuDeviceShr.rtti,    // [0]: (kernelhostvgpudeviceshr) this
        &__nvoc_metadata__KernelHostVgpuDeviceShr.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__KernelHostVgpuDeviceShr.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHostVgpuDeviceShr = 
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

// Initialize newly constructed object.
void __nvoc_init__KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_KernelHostVgpuDeviceShr = pThis;    // (kernelhostvgpudeviceshr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceShr.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceShr.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceShr;    // (kernelhostvgpudeviceshr) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelHostVgpuDeviceShr(pThis);
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
char __nvoc_class_id_uniqueness_check__0xb12d7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for KernelHostVgpuDeviceApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
void __nvoc_init_funcTable_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
NV_STATUS __nvoc_ctor_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);
void __nvoc_dtor_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelHostVgpuDeviceApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHostVgpuDeviceApi;

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelHostVgpuDeviceApi,
    /*pExportInfo=*/        &__nvoc_export_info__KernelHostVgpuDeviceApi
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


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelHostVgpuDeviceApi __nvoc_metadata__KernelHostVgpuDeviceApi = {
    .rtti.pClassDef = &__nvoc_class_def_KernelHostVgpuDeviceApi,    // (kernelhostvgpudeviceapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHostVgpuDeviceApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__kernelhostvgpudeviceapiCanCopy__ = &kernelhostvgpudeviceapiCanCopy_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_KernelHostVgpuDeviceApi_resCanCopy,    // virtual
    .vtable.__kernelhostvgpudeviceapiControl__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__kernelhostvgpudeviceapiMap__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__kernelhostvgpudeviceapiUnmap__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__kernelhostvgpudeviceapiShareCallback__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kernelhostvgpudeviceapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiAccessCallback__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__kernelhostvgpudeviceapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__kernelhostvgpudeviceapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__kernelhostvgpudeviceapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kernelhostvgpudeviceapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__kernelhostvgpudeviceapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiPreDestruct__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiControlFilter__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__kernelhostvgpudeviceapiMapTo__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetRefCount__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kernelhostvgpudeviceapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__kernelhostvgpudeviceapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__kernelhostvgpudeviceapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__kernelhostvgpudeviceapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__kernelhostvgpudeviceapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__kernelhostvgpudeviceapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_kernelhostvgpudeviceapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelHostVgpuDeviceApi = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__KernelHostVgpuDeviceApi.rtti,    // [0]: (kernelhostvgpudeviceapi) this
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in KernelHostVgpuDeviceApi from superclasses

// kernelhostvgpudeviceapiCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_KernelHostVgpuDeviceApi_resCanCopy(struct RsResource *pKernelHostVgpuDeviceApi) {
    return kernelhostvgpudeviceapiCanCopy((struct KernelHostVgpuDeviceApi *)(((unsigned char *) pKernelHostVgpuDeviceApi) - NV_OFFSETOF(KernelHostVgpuDeviceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


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


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHostVgpuDeviceApi = 
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

    // Initialize vtable(s) with 12 per-object function pointer(s).
    __nvoc_init_funcTable_KernelHostVgpuDeviceApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_KernelHostVgpuDeviceApi = pThis;    // (kernelhostvgpudeviceapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelHostVgpuDeviceApi;    // (kernelhostvgpudeviceapi) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelHostVgpuDeviceApi(pThis);
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

