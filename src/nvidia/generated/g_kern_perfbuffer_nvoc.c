#define NVOC_KERN_PERFBUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_perfbuffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4bc43b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PerfBuffer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_PerfBuffer(PerfBuffer*, RmHalspecOwner* );
void __nvoc_init_funcTable_PerfBuffer(PerfBuffer*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_PerfBuffer(PerfBuffer*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_PerfBuffer(PerfBuffer*, RmHalspecOwner* );
void __nvoc_dtor_PerfBuffer(PerfBuffer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_PerfBuffer;

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_PerfBuffer = {
    /*pClassDef=*/          &__nvoc_class_def_PerfBuffer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PerfBuffer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_PerfBuffer = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_PerfBuffer_PerfBuffer,
        &__nvoc_rtti_PerfBuffer_GpuResource,
        &__nvoc_rtti_PerfBuffer_RmResource,
        &__nvoc_rtti_PerfBuffer_RmResourceCommon,
        &__nvoc_rtti_PerfBuffer_RsResource,
        &__nvoc_rtti_PerfBuffer_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_PerfBuffer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(PerfBuffer),
        /*classId=*/            classId(PerfBuffer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "PerfBuffer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_PerfBuffer,
    /*pCastInfo=*/          &__nvoc_castinfo_PerfBuffer,
    /*pExportInfo=*/        &__nvoc_export_info_PerfBuffer
};

// 25 up-thunk(s) defined to bridge methods in PerfBuffer to superclasses

// perfbufferControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_perfbufferControl(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, pParams);
}

// perfbufferMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_perfbufferMap(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// perfbufferUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_perfbufferUnmap(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, pCpuMapping);
}

// perfbufferShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_perfbufferShareCallback(struct PerfBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// perfbufferGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_perfbufferGetRegBaseOffsetAndSize(struct PerfBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pGpu, pOffset, pSize);
}

// perfbufferGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_perfbufferGetMapAddrSpace(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// perfbufferInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_perfbufferInternalControlForward(struct PerfBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), command, pParams, size);
}

// perfbufferGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_perfbufferGetInternalObjectHandle(struct PerfBuffer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset));
}

// perfbufferAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_perfbufferAccessCallback(struct PerfBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// perfbufferGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_perfbufferGetMemInterMapParams(struct PerfBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pParams);
}

// perfbufferCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_perfbufferCheckMemInterUnmap(struct PerfBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

// perfbufferGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_perfbufferGetMemoryMappingDescriptor(struct PerfBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), ppMemDesc);
}

// perfbufferControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_perfbufferControlSerialization_Prologue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

// perfbufferControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_perfbufferControlSerialization_Epilogue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

// perfbufferControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_perfbufferControl_Prologue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

// perfbufferControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_perfbufferControl_Epilogue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

// perfbufferCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_perfbufferCanCopy(struct PerfBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

// perfbufferIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_perfbufferIsDuplicate(struct PerfBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), hMemory, pDuplicate);
}

// perfbufferPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_perfbufferPreDestruct(struct PerfBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

// perfbufferControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_perfbufferControlFilter(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pCallContext, pParams);
}

// perfbufferIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_perfbufferIsPartialUnmapSupported(struct PerfBuffer *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

// perfbufferMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_perfbufferMapTo(struct PerfBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pParams);
}

// perfbufferUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_perfbufferUnmapFrom(struct PerfBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pParams);
}

// perfbufferGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_perfbufferGetRefCount(struct PerfBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

// perfbufferAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_perfbufferAddAdditionalDependants(struct RsClient *pClient, struct PerfBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_PerfBuffer = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_PerfBuffer(PerfBuffer *pThis) {
    __nvoc_perfbufferDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_PerfBuffer_fail_GpuResource;
    __nvoc_init_dataField_PerfBuffer(pThis, pRmhalspecowner);

    status = __nvoc_perfbufferConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_PerfBuffer_fail__init;
    goto __nvoc_ctor_PerfBuffer_exit; // Success

__nvoc_ctor_PerfBuffer_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_PerfBuffer_fail_GpuResource:
__nvoc_ctor_PerfBuffer_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PerfBuffer_1(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // perfbufferConstructHal -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__perfbufferConstructHal__ = &perfbufferConstructHal_46f6a7;
    }
    else
    {
        pThis->__perfbufferConstructHal__ = &perfbufferConstructHal_KERNEL;
    }

    // perfbufferControl -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferControl__ = &__nvoc_up_thunk_GpuResource_perfbufferControl;

    // perfbufferMap -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferMap__ = &__nvoc_up_thunk_GpuResource_perfbufferMap;

    // perfbufferUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferUnmap__ = &__nvoc_up_thunk_GpuResource_perfbufferUnmap;

    // perfbufferShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferShareCallback__ = &__nvoc_up_thunk_GpuResource_perfbufferShareCallback;

    // perfbufferGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_perfbufferGetRegBaseOffsetAndSize;

    // perfbufferGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_perfbufferGetMapAddrSpace;

    // perfbufferInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferInternalControlForward__ = &__nvoc_up_thunk_GpuResource_perfbufferInternalControlForward;

    // perfbufferGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__perfbufferGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_perfbufferGetInternalObjectHandle;

    // perfbufferAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferAccessCallback__ = &__nvoc_up_thunk_RmResource_perfbufferAccessCallback;

    // perfbufferGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_perfbufferGetMemInterMapParams;

    // perfbufferCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_perfbufferCheckMemInterUnmap;

    // perfbufferGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_perfbufferGetMemoryMappingDescriptor;

    // perfbufferControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_perfbufferControlSerialization_Prologue;

    // perfbufferControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_perfbufferControlSerialization_Epilogue;

    // perfbufferControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferControl_Prologue__ = &__nvoc_up_thunk_RmResource_perfbufferControl_Prologue;

    // perfbufferControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__perfbufferControl_Epilogue__ = &__nvoc_up_thunk_RmResource_perfbufferControl_Epilogue;

    // perfbufferCanCopy -- virtual inherited (res) base (gpures)
    pThis->__perfbufferCanCopy__ = &__nvoc_up_thunk_RsResource_perfbufferCanCopy;

    // perfbufferIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__perfbufferIsDuplicate__ = &__nvoc_up_thunk_RsResource_perfbufferIsDuplicate;

    // perfbufferPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__perfbufferPreDestruct__ = &__nvoc_up_thunk_RsResource_perfbufferPreDestruct;

    // perfbufferControlFilter -- virtual inherited (res) base (gpures)
    pThis->__perfbufferControlFilter__ = &__nvoc_up_thunk_RsResource_perfbufferControlFilter;

    // perfbufferIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__perfbufferIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_perfbufferIsPartialUnmapSupported;

    // perfbufferMapTo -- virtual inherited (res) base (gpures)
    pThis->__perfbufferMapTo__ = &__nvoc_up_thunk_RsResource_perfbufferMapTo;

    // perfbufferUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__perfbufferUnmapFrom__ = &__nvoc_up_thunk_RsResource_perfbufferUnmapFrom;

    // perfbufferGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__perfbufferGetRefCount__ = &__nvoc_up_thunk_RsResource_perfbufferGetRefCount;

    // perfbufferAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__perfbufferAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_perfbufferAddAdditionalDependants;
} // End __nvoc_init_funcTable_PerfBuffer_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_PerfBuffer_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_PerfBuffer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_PerfBuffer(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_PerfBuffer(PerfBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    PerfBuffer *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(PerfBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(PerfBuffer));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_PerfBuffer);

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

    __nvoc_init_PerfBuffer(pThis, pRmhalspecowner);
    status = __nvoc_ctor_PerfBuffer(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_PerfBuffer_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_PerfBuffer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(PerfBuffer));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_PerfBuffer(PerfBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_PerfBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

