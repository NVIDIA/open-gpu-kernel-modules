#define NVOC_UVM_CHANNEL_RETAINER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_uvm_channel_retainer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xa3f03a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UvmChannelRetainer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_UvmChannelRetainer(UvmChannelRetainer*, RmHalspecOwner* );
void __nvoc_init_funcTable_UvmChannelRetainer(UvmChannelRetainer*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_UvmChannelRetainer(UvmChannelRetainer*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_UvmChannelRetainer(UvmChannelRetainer*, RmHalspecOwner* );
void __nvoc_dtor_UvmChannelRetainer(UvmChannelRetainer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_UvmChannelRetainer;

static const struct NVOC_RTTI __nvoc_rtti_UvmChannelRetainer_UvmChannelRetainer = {
    /*pClassDef=*/          &__nvoc_class_def_UvmChannelRetainer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_UvmChannelRetainer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_UvmChannelRetainer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmChannelRetainer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmChannelRetainer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmChannelRetainer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmChannelRetainer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmChannelRetainer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmChannelRetainer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmChannelRetainer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmChannelRetainer_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmChannelRetainer, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_UvmChannelRetainer = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_UvmChannelRetainer_UvmChannelRetainer,
        &__nvoc_rtti_UvmChannelRetainer_GpuResource,
        &__nvoc_rtti_UvmChannelRetainer_RmResource,
        &__nvoc_rtti_UvmChannelRetainer_RmResourceCommon,
        &__nvoc_rtti_UvmChannelRetainer_RsResource,
        &__nvoc_rtti_UvmChannelRetainer_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_UvmChannelRetainer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(UvmChannelRetainer),
        /*classId=*/            classId(UvmChannelRetainer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "UvmChannelRetainer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_UvmChannelRetainer,
    /*pCastInfo=*/          &__nvoc_castinfo_UvmChannelRetainer,
    /*pExportInfo=*/        &__nvoc_export_info_UvmChannelRetainer
};

// 25 up-thunk(s) defined to bridge methods in UvmChannelRetainer to superclasses

// uvmchanrtnrControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmchanrtnrControl(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), pCallContext, pParams);
}

// uvmchanrtnrMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmchanrtnrMap(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// uvmchanrtnrUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmchanrtnrUnmap(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), pCallContext, pCpuMapping);
}

// uvmchanrtnrShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_uvmchanrtnrShareCallback(struct UvmChannelRetainer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// uvmchanrtnrGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmchanrtnrGetRegBaseOffsetAndSize(struct UvmChannelRetainer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), pGpu, pOffset, pSize);
}

// uvmchanrtnrGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmchanrtnrGetMapAddrSpace(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// uvmchanrtnrInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmchanrtnrInternalControlForward(struct UvmChannelRetainer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset), command, pParams, size);
}

// uvmchanrtnrGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_uvmchanrtnrGetInternalObjectHandle(struct UvmChannelRetainer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmChannelRetainer_GpuResource.offset));
}

// uvmchanrtnrAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_uvmchanrtnrAccessCallback(struct UvmChannelRetainer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// uvmchanrtnrGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmchanrtnrGetMemInterMapParams(struct UvmChannelRetainer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), pParams);
}

// uvmchanrtnrCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmchanrtnrCheckMemInterUnmap(struct UvmChannelRetainer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), bSubdeviceHandleProvided);
}

// uvmchanrtnrGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmchanrtnrGetMemoryMappingDescriptor(struct UvmChannelRetainer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), ppMemDesc);
}

// uvmchanrtnrControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmchanrtnrControlSerialization_Prologue(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), pCallContext, pParams);
}

// uvmchanrtnrControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_uvmchanrtnrControlSerialization_Epilogue(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), pCallContext, pParams);
}

// uvmchanrtnrControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmchanrtnrControl_Prologue(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), pCallContext, pParams);
}

// uvmchanrtnrControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_uvmchanrtnrControl_Epilogue(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RmResource.offset), pCallContext, pParams);
}

// uvmchanrtnrCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_uvmchanrtnrCanCopy(struct UvmChannelRetainer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset));
}

// uvmchanrtnrIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmchanrtnrIsDuplicate(struct UvmChannelRetainer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset), hMemory, pDuplicate);
}

// uvmchanrtnrPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_uvmchanrtnrPreDestruct(struct UvmChannelRetainer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset));
}

// uvmchanrtnrControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmchanrtnrControlFilter(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset), pCallContext, pParams);
}

// uvmchanrtnrIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_uvmchanrtnrIsPartialUnmapSupported(struct UvmChannelRetainer *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset));
}

// uvmchanrtnrMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmchanrtnrMapTo(struct UvmChannelRetainer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset), pParams);
}

// uvmchanrtnrUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmchanrtnrUnmapFrom(struct UvmChannelRetainer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset), pParams);
}

// uvmchanrtnrGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_uvmchanrtnrGetRefCount(struct UvmChannelRetainer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset));
}

// uvmchanrtnrAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_uvmchanrtnrAddAdditionalDependants(struct RsClient *pClient, struct UvmChannelRetainer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmChannelRetainer_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_UvmChannelRetainer = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_UvmChannelRetainer(UvmChannelRetainer *pThis) {
    __nvoc_uvmchanrtnrDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_UvmChannelRetainer(UvmChannelRetainer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_UvmChannelRetainer(UvmChannelRetainer *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_UvmChannelRetainer_fail_GpuResource;
    __nvoc_init_dataField_UvmChannelRetainer(pThis, pRmhalspecowner);

    status = __nvoc_uvmchanrtnrConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_UvmChannelRetainer_fail__init;
    goto __nvoc_ctor_UvmChannelRetainer_exit; // Success

__nvoc_ctor_UvmChannelRetainer_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_UvmChannelRetainer_fail_GpuResource:
__nvoc_ctor_UvmChannelRetainer_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_UvmChannelRetainer_1(UvmChannelRetainer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // uvmchanrtnrControl -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrControl__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrControl;

    // uvmchanrtnrMap -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrMap__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrMap;

    // uvmchanrtnrUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrUnmap__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrUnmap;

    // uvmchanrtnrShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrShareCallback__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrShareCallback;

    // uvmchanrtnrGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrGetRegBaseOffsetAndSize;

    // uvmchanrtnrGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrGetMapAddrSpace;

    // uvmchanrtnrInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrInternalControlForward__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrInternalControlForward;

    // uvmchanrtnrGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__uvmchanrtnrGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_uvmchanrtnrGetInternalObjectHandle;

    // uvmchanrtnrAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrAccessCallback__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrAccessCallback;

    // uvmchanrtnrGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrGetMemInterMapParams;

    // uvmchanrtnrCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrCheckMemInterUnmap;

    // uvmchanrtnrGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrGetMemoryMappingDescriptor;

    // uvmchanrtnrControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrControlSerialization_Prologue;

    // uvmchanrtnrControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrControlSerialization_Epilogue;

    // uvmchanrtnrControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrControl_Prologue__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrControl_Prologue;

    // uvmchanrtnrControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__uvmchanrtnrControl_Epilogue__ = &__nvoc_up_thunk_RmResource_uvmchanrtnrControl_Epilogue;

    // uvmchanrtnrCanCopy -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrCanCopy__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrCanCopy;

    // uvmchanrtnrIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrIsDuplicate__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrIsDuplicate;

    // uvmchanrtnrPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrPreDestruct__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrPreDestruct;

    // uvmchanrtnrControlFilter -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrControlFilter__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrControlFilter;

    // uvmchanrtnrIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__uvmchanrtnrIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrIsPartialUnmapSupported;

    // uvmchanrtnrMapTo -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrMapTo__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrMapTo;

    // uvmchanrtnrUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrUnmapFrom__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrUnmapFrom;

    // uvmchanrtnrGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrGetRefCount__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrGetRefCount;

    // uvmchanrtnrAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__uvmchanrtnrAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_uvmchanrtnrAddAdditionalDependants;
} // End __nvoc_init_funcTable_UvmChannelRetainer_1 with approximately 25 basic block(s).


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_UvmChannelRetainer(UvmChannelRetainer *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 25 per-object function pointer(s).
    __nvoc_init_funcTable_UvmChannelRetainer_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_UvmChannelRetainer(UvmChannelRetainer *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_UvmChannelRetainer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_UvmChannelRetainer(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_UvmChannelRetainer(UvmChannelRetainer **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    UvmChannelRetainer *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(UvmChannelRetainer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(UvmChannelRetainer));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_UvmChannelRetainer);

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

    __nvoc_init_UvmChannelRetainer(pThis, pRmhalspecowner);
    status = __nvoc_ctor_UvmChannelRetainer(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_UvmChannelRetainer_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_UvmChannelRetainer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(UvmChannelRetainer));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_UvmChannelRetainer(UvmChannelRetainer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_UvmChannelRetainer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

