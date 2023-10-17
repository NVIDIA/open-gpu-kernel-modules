#define NVOC_USERMODE_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_usermode_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x6f57ec = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UserModeApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_UserModeApi(UserModeApi*);
void __nvoc_init_funcTable_UserModeApi(UserModeApi*);
NV_STATUS __nvoc_ctor_UserModeApi(UserModeApi*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_UserModeApi(UserModeApi*);
void __nvoc_dtor_UserModeApi(UserModeApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_UserModeApi;

static const struct NVOC_RTTI __nvoc_rtti_UserModeApi_UserModeApi = {
    /*pClassDef=*/          &__nvoc_class_def_UserModeApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_UserModeApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_UserModeApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserModeApi, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_UserModeApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserModeApi, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UserModeApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserModeApi, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_UserModeApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserModeApi, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UserModeApi_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserModeApi, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_UserModeApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_UserModeApi_UserModeApi,
        &__nvoc_rtti_UserModeApi_Memory,
        &__nvoc_rtti_UserModeApi_RmResource,
        &__nvoc_rtti_UserModeApi_RmResourceCommon,
        &__nvoc_rtti_UserModeApi_RsResource,
        &__nvoc_rtti_UserModeApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_UserModeApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(UserModeApi),
        /*classId=*/            classId(UserModeApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "UserModeApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_UserModeApi,
    /*pCastInfo=*/          &__nvoc_castinfo_UserModeApi,
    /*pExportInfo=*/        &__nvoc_export_info_UserModeApi
};

static NvBool __nvoc_thunk_UserModeApi_resCanCopy(struct RsResource *pUserModeApi) {
    return usrmodeCanCopy((struct UserModeApi *)(((unsigned char *)pUserModeApi) - __nvoc_rtti_UserModeApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_UserModeApi_memGetMemInterMapParams(struct Memory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return usrmodeGetMemInterMapParams((struct UserModeApi *)(((unsigned char *)pMemory) - __nvoc_rtti_UserModeApi_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeCheckMemInterUnmap(struct UserModeApi *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_usrmodeShareCallback(struct UserModeApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_usrmodeMapTo(struct UserModeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeGetMapAddrSpace(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_usrmodeIsExportAllowed(struct UserModeApi *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_usrmodeGetRefCount(struct UserModeApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_usrmodeAddAdditionalDependants(struct RsClient *pClient, struct UserModeApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_usrmodeControl_Prologue(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_usrmodeIsGpuMapAllowed(struct UserModeApi *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_usrmodeUnmapFrom(struct UserModeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_usrmodeControl_Epilogue(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_usrmodeControlLookup(struct UserModeApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeControl(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeUnmap(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeGetMemoryMappingDescriptor(struct UserModeApi *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_usrmodeControlFilter(struct UserModeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_usrmodeControlSerialization_Prologue(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeIsReady(struct UserModeApi *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeCheckCopyPermissions(struct UserModeApi *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_usrmodePreDestruct(struct UserModeApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeIsDuplicate(struct UserModeApi *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_usrmodeControlSerialization_Epilogue(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_usrmodeMap(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_UserModeApi_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_usrmodeAccessCallback(struct UserModeApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_UserModeApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_UserModeApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_UserModeApi(UserModeApi *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_UserModeApi(UserModeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_UserModeApi(UserModeApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_UserModeApi_fail_Memory;
    __nvoc_init_dataField_UserModeApi(pThis);

    status = __nvoc_usrmodeConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_UserModeApi_fail__init;
    goto __nvoc_ctor_UserModeApi_exit; // Success

__nvoc_ctor_UserModeApi_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_UserModeApi_fail_Memory:
__nvoc_ctor_UserModeApi_exit:

    return status;
}

static void __nvoc_init_funcTable_UserModeApi_1(UserModeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__usrmodeCanCopy__ = &usrmodeCanCopy_IMPL;

    pThis->__usrmodeGetMemInterMapParams__ = &usrmodeGetMemInterMapParams_IMPL;

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_UserModeApi_resCanCopy;

    pThis->__nvoc_base_Memory.__memGetMemInterMapParams__ = &__nvoc_thunk_UserModeApi_memGetMemInterMapParams;

    pThis->__usrmodeCheckMemInterUnmap__ = &__nvoc_thunk_Memory_usrmodeCheckMemInterUnmap;

    pThis->__usrmodeShareCallback__ = &__nvoc_thunk_RmResource_usrmodeShareCallback;

    pThis->__usrmodeMapTo__ = &__nvoc_thunk_RsResource_usrmodeMapTo;

    pThis->__usrmodeGetMapAddrSpace__ = &__nvoc_thunk_Memory_usrmodeGetMapAddrSpace;

    pThis->__usrmodeIsExportAllowed__ = &__nvoc_thunk_Memory_usrmodeIsExportAllowed;

    pThis->__usrmodeGetRefCount__ = &__nvoc_thunk_RsResource_usrmodeGetRefCount;

    pThis->__usrmodeAddAdditionalDependants__ = &__nvoc_thunk_RsResource_usrmodeAddAdditionalDependants;

    pThis->__usrmodeControl_Prologue__ = &__nvoc_thunk_RmResource_usrmodeControl_Prologue;

    pThis->__usrmodeIsGpuMapAllowed__ = &__nvoc_thunk_Memory_usrmodeIsGpuMapAllowed;

    pThis->__usrmodeUnmapFrom__ = &__nvoc_thunk_RsResource_usrmodeUnmapFrom;

    pThis->__usrmodeControl_Epilogue__ = &__nvoc_thunk_RmResource_usrmodeControl_Epilogue;

    pThis->__usrmodeControlLookup__ = &__nvoc_thunk_RsResource_usrmodeControlLookup;

    pThis->__usrmodeControl__ = &__nvoc_thunk_Memory_usrmodeControl;

    pThis->__usrmodeUnmap__ = &__nvoc_thunk_Memory_usrmodeUnmap;

    pThis->__usrmodeGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_usrmodeGetMemoryMappingDescriptor;

    pThis->__usrmodeControlFilter__ = &__nvoc_thunk_RsResource_usrmodeControlFilter;

    pThis->__usrmodeControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_usrmodeControlSerialization_Prologue;

    pThis->__usrmodeIsReady__ = &__nvoc_thunk_Memory_usrmodeIsReady;

    pThis->__usrmodeCheckCopyPermissions__ = &__nvoc_thunk_Memory_usrmodeCheckCopyPermissions;

    pThis->__usrmodePreDestruct__ = &__nvoc_thunk_RsResource_usrmodePreDestruct;

    pThis->__usrmodeIsDuplicate__ = &__nvoc_thunk_Memory_usrmodeIsDuplicate;

    pThis->__usrmodeControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_usrmodeControlSerialization_Epilogue;

    pThis->__usrmodeMap__ = &__nvoc_thunk_Memory_usrmodeMap;

    pThis->__usrmodeAccessCallback__ = &__nvoc_thunk_RmResource_usrmodeAccessCallback;
}

void __nvoc_init_funcTable_UserModeApi(UserModeApi *pThis) {
    __nvoc_init_funcTable_UserModeApi_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_UserModeApi(UserModeApi *pThis) {
    pThis->__nvoc_pbase_UserModeApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_UserModeApi(pThis);
}

NV_STATUS __nvoc_objCreate_UserModeApi(UserModeApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    UserModeApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(UserModeApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(UserModeApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_UserModeApi);

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

    __nvoc_init_UserModeApi(pThis);
    status = __nvoc_ctor_UserModeApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_UserModeApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_UserModeApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(UserModeApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_UserModeApi(UserModeApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_UserModeApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

