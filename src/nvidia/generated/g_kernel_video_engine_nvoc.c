#define NVOC_KERNEL_VIDEO_ENGINE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_video_engine_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__9e2f3e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVideoEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for KernelVideoEngine
void __nvoc_init__Object(Object*);
void __nvoc_init__KernelVideoEngine(KernelVideoEngine*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelVideoEngine(KernelVideoEngine*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelVideoEngine(KernelVideoEngine*, RmHalspecOwner *pRmhalspecowner, struct OBJGPU *pGpu, ENGDESCRIPTOR physEngDesc);
void __nvoc_init_dataField_KernelVideoEngine(KernelVideoEngine*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelVideoEngine(KernelVideoEngine*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelVideoEngine;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelVideoEngine;

// Down-thunk(s) to bridge KernelVideoEngine methods from ancestors (if any)

// Up-thunk(s) to bridge KernelVideoEngine methods to ancestors (if any)

// Class-specific details for KernelVideoEngine
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVideoEngine = 
{
    .classInfo.size =               sizeof(KernelVideoEngine),
    .classInfo.classId =            classId(KernelVideoEngine),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelVideoEngine",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelVideoEngine,
    .pCastInfo =          &__nvoc_castinfo__KernelVideoEngine,
    .pExportInfo =        &__nvoc_export_info__KernelVideoEngine
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelVideoEngine __nvoc_metadata__KernelVideoEngine = {
    .rtti.pClassDef = &__nvoc_class_def_KernelVideoEngine,    // (kvideng) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelVideoEngine,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(KernelVideoEngine, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelVideoEngine = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__KernelVideoEngine.rtti,    // [0]: (kvideng) this
        &__nvoc_metadata__KernelVideoEngine.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelVideoEngine = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelVideoEngine object.
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_KernelVideoEngine(KernelVideoEngine* pThis) {

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_KernelVideoEngine(KernelVideoEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct KernelVideoEngine object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_KernelVideoEngine(KernelVideoEngine *pKernelVideoEngine, RmHalspecOwner *pRmhalspecowner, struct OBJGPU *pGpu, ENGDESCRIPTOR physEngDesc) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pKernelVideoEngine->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_KernelVideoEngine_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_KernelVideoEngine(pKernelVideoEngine, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_kvidengConstruct(pKernelVideoEngine, pGpu, physEngDesc);
    if (status != NV_OK) goto __nvoc_ctor_KernelVideoEngine_fail__init;
    goto __nvoc_ctor_KernelVideoEngine_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelVideoEngine_fail__init:
    __nvoc_dtor_Object(&pKernelVideoEngine->__nvoc_base_Object);
__nvoc_ctor_KernelVideoEngine_fail_Object:
__nvoc_ctor_KernelVideoEngine_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelVideoEngine_1(KernelVideoEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
} // End __nvoc_init_funcTable_KernelVideoEngine_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelVideoEngine(KernelVideoEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelVideoEngine_1(pThis, pRmhalspecowner);
}

NvBool kvidengIsVideoTraceLogSupported_STATIC_DISPATCH(struct OBJGPU *pGpu) {
    ChipHal *chipHal = &staticCast(pGpu, GpuHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;

    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        return kvidengIsVideoTraceLogSupported_IMPL(pGpu);
    }
    // default
    else
    {
        return kvidengIsVideoTraceLogSupported_d69453(pGpu);
    }

    NV_ASSERT_FAILED("No hal impl found for kvidengIsVideoTraceLogSupported");

    return NV_FALSE;
}

// Initialize newly constructed object.
void __nvoc_init__KernelVideoEngine(KernelVideoEngine *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_KernelVideoEngine = pThis;    // (kvideng) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelVideoEngine.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelVideoEngine;    // (kvideng) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelVideoEngine(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelVideoEngine(KernelVideoEngine **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct OBJGPU *pGpu, ENGDESCRIPTOR physEngDesc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelVideoEngine *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelVideoEngine));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelVideoEngine));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelVideoEngine_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelVideoEngine_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelVideoEngine(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelVideoEngine(__nvoc_pThis, pRmhalspecowner, pGpu, physEngDesc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelVideoEngine_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelVideoEngine_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelVideoEngine));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelVideoEngine(KernelVideoEngine **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct OBJGPU *pGpu = va_arg(__nvoc_args, struct OBJGPU *);
    ENGDESCRIPTOR physEngDesc = va_arg(__nvoc_args, ENGDESCRIPTOR);

    __nvoc_status = __nvoc_objCreate_KernelVideoEngine(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pGpu, physEngDesc);

    return __nvoc_status;
}

