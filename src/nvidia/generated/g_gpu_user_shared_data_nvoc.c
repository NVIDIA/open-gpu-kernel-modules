#define NVOC_GPU_USER_SHARED_DATA_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_user_shared_data_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5e7d1f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_GpuUserSharedData(GpuUserSharedData*);
void __nvoc_init_funcTable_GpuUserSharedData(GpuUserSharedData*);
NV_STATUS __nvoc_ctor_GpuUserSharedData(GpuUserSharedData*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GpuUserSharedData(GpuUserSharedData*);
void __nvoc_dtor_GpuUserSharedData(GpuUserSharedData*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuUserSharedData;

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_GpuUserSharedData = {
    /*pClassDef=*/          &__nvoc_class_def_GpuUserSharedData,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuUserSharedData,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GpuUserSharedData = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_GpuUserSharedData_GpuUserSharedData,
        &__nvoc_rtti_GpuUserSharedData_Memory,
        &__nvoc_rtti_GpuUserSharedData_RmResource,
        &__nvoc_rtti_GpuUserSharedData_RmResourceCommon,
        &__nvoc_rtti_GpuUserSharedData_RsResource,
        &__nvoc_rtti_GpuUserSharedData_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuUserSharedData),
        /*classId=*/            classId(GpuUserSharedData),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuUserSharedData",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuUserSharedData,
    /*pCastInfo=*/          &__nvoc_castinfo_GpuUserSharedData,
    /*pExportInfo=*/        &__nvoc_export_info_GpuUserSharedData
};

static NvBool __nvoc_thunk_GpuUserSharedData_resCanCopy(struct RsResource *pData) {
    return gpushareddataCanCopy((struct GpuUserSharedData *)(((unsigned char *)pData) - __nvoc_rtti_GpuUserSharedData_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataCheckMemInterUnmap(struct GpuUserSharedData *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_gpushareddataShareCallback(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataMapTo(struct GpuUserSharedData *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataGetMapAddrSpace(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_gpushareddataIsExportAllowed(struct GpuUserSharedData *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_gpushareddataGetRefCount(struct GpuUserSharedData *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset));
}

static void __nvoc_thunk_RsResource_gpushareddataAddAdditionalDependants(struct RsClient *pClient, struct GpuUserSharedData *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_gpushareddataControl_Prologue(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_gpushareddataIsGpuMapAllowed(struct GpuUserSharedData *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataUnmapFrom(struct GpuUserSharedData *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_gpushareddataControl_Epilogue(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataControlLookup(struct GpuUserSharedData *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataControl(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataUnmap(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataGetMemInterMapParams(struct GpuUserSharedData *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataGetMemoryMappingDescriptor(struct GpuUserSharedData *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataControlFilter(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_gpushareddataControlSerialization_Prologue(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataIsReady(struct GpuUserSharedData *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataCheckCopyPermissions(struct GpuUserSharedData *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_gpushareddataPreDestruct(struct GpuUserSharedData *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataIsDuplicate(struct GpuUserSharedData *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_gpushareddataControlSerialization_Epilogue(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_gpushareddataMap(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_GpuUserSharedData_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_gpushareddataAccessCallback(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GpuUserSharedData[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gpushareddataCtrlCmdRequestDataPoll_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xde0001u,
        /*paramSize=*/  sizeof(NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GpuUserSharedData.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gpushareddataCtrlCmdRequestDataPoll"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuUserSharedData = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_GpuUserSharedData
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_GpuUserSharedData(GpuUserSharedData *pThis) {
    __nvoc_gpushareddataDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GpuUserSharedData(GpuUserSharedData *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GpuUserSharedData(GpuUserSharedData *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuUserSharedData_fail_Memory;
    __nvoc_init_dataField_GpuUserSharedData(pThis);

    status = __nvoc_gpushareddataConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuUserSharedData_fail__init;
    goto __nvoc_ctor_GpuUserSharedData_exit; // Success

__nvoc_ctor_GpuUserSharedData_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_GpuUserSharedData_fail_Memory:
__nvoc_ctor_GpuUserSharedData_exit:

    return status;
}

static void __nvoc_init_funcTable_GpuUserSharedData_1(GpuUserSharedData *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__gpushareddataCanCopy__ = &gpushareddataCanCopy_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__gpushareddataCtrlCmdRequestDataPoll__ = &gpushareddataCtrlCmdRequestDataPoll_IMPL;
#endif

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_GpuUserSharedData_resCanCopy;

    pThis->__gpushareddataCheckMemInterUnmap__ = &__nvoc_thunk_Memory_gpushareddataCheckMemInterUnmap;

    pThis->__gpushareddataShareCallback__ = &__nvoc_thunk_RmResource_gpushareddataShareCallback;

    pThis->__gpushareddataMapTo__ = &__nvoc_thunk_RsResource_gpushareddataMapTo;

    pThis->__gpushareddataGetMapAddrSpace__ = &__nvoc_thunk_Memory_gpushareddataGetMapAddrSpace;

    pThis->__gpushareddataIsExportAllowed__ = &__nvoc_thunk_Memory_gpushareddataIsExportAllowed;

    pThis->__gpushareddataGetRefCount__ = &__nvoc_thunk_RsResource_gpushareddataGetRefCount;

    pThis->__gpushareddataAddAdditionalDependants__ = &__nvoc_thunk_RsResource_gpushareddataAddAdditionalDependants;

    pThis->__gpushareddataControl_Prologue__ = &__nvoc_thunk_RmResource_gpushareddataControl_Prologue;

    pThis->__gpushareddataIsGpuMapAllowed__ = &__nvoc_thunk_Memory_gpushareddataIsGpuMapAllowed;

    pThis->__gpushareddataUnmapFrom__ = &__nvoc_thunk_RsResource_gpushareddataUnmapFrom;

    pThis->__gpushareddataControl_Epilogue__ = &__nvoc_thunk_RmResource_gpushareddataControl_Epilogue;

    pThis->__gpushareddataControlLookup__ = &__nvoc_thunk_RsResource_gpushareddataControlLookup;

    pThis->__gpushareddataControl__ = &__nvoc_thunk_Memory_gpushareddataControl;

    pThis->__gpushareddataUnmap__ = &__nvoc_thunk_Memory_gpushareddataUnmap;

    pThis->__gpushareddataGetMemInterMapParams__ = &__nvoc_thunk_Memory_gpushareddataGetMemInterMapParams;

    pThis->__gpushareddataGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_gpushareddataGetMemoryMappingDescriptor;

    pThis->__gpushareddataControlFilter__ = &__nvoc_thunk_RsResource_gpushareddataControlFilter;

    pThis->__gpushareddataControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_gpushareddataControlSerialization_Prologue;

    pThis->__gpushareddataIsReady__ = &__nvoc_thunk_Memory_gpushareddataIsReady;

    pThis->__gpushareddataCheckCopyPermissions__ = &__nvoc_thunk_Memory_gpushareddataCheckCopyPermissions;

    pThis->__gpushareddataPreDestruct__ = &__nvoc_thunk_RsResource_gpushareddataPreDestruct;

    pThis->__gpushareddataIsDuplicate__ = &__nvoc_thunk_Memory_gpushareddataIsDuplicate;

    pThis->__gpushareddataControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_gpushareddataControlSerialization_Epilogue;

    pThis->__gpushareddataMap__ = &__nvoc_thunk_Memory_gpushareddataMap;

    pThis->__gpushareddataAccessCallback__ = &__nvoc_thunk_RmResource_gpushareddataAccessCallback;
}

void __nvoc_init_funcTable_GpuUserSharedData(GpuUserSharedData *pThis) {
    __nvoc_init_funcTable_GpuUserSharedData_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_GpuUserSharedData(GpuUserSharedData *pThis) {
    pThis->__nvoc_pbase_GpuUserSharedData = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_GpuUserSharedData(pThis);
}

NV_STATUS __nvoc_objCreate_GpuUserSharedData(GpuUserSharedData **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    GpuUserSharedData *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GpuUserSharedData), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(GpuUserSharedData));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GpuUserSharedData);

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_GpuUserSharedData(pThis);
    status = __nvoc_ctor_GpuUserSharedData(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GpuUserSharedData_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GpuUserSharedData_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GpuUserSharedData));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuUserSharedData(GpuUserSharedData **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GpuUserSharedData(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

