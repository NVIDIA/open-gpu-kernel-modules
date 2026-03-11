#define NVOC_GPU_ARCH_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_arch_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4b33af = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuArch;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuHalspecOwner;

// Forward declarations for GpuArch
void __nvoc_init__Object(Object*);
void __nvoc_init__GpuHalspecOwner(GpuHalspecOwner*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType);
void __nvoc_init__GpuArch(GpuArch*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType);
void __nvoc_init_funcTable_GpuArch(GpuArch*);
NV_STATUS __nvoc_ctor_GpuArch(GpuArch*, NvU32 chipArch, NvU32 chipImpl, NvU32 hidrev, TEGRA_CHIP_TYPE tegraType);
void __nvoc_init_dataField_GpuArch(GpuArch*);
void __nvoc_dtor_GpuArch(GpuArch*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuArch;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuArch;

// Down-thunk(s) to bridge GpuArch methods from ancestors (if any)

// Up-thunk(s) to bridge GpuArch methods to ancestors (if any)

// Class-specific details for GpuArch
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuArch = 
{
    .classInfo.size =               sizeof(GpuArch),
    .classInfo.classId =            classId(GpuArch),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuArch",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuArch,
    .pCastInfo =          &__nvoc_castinfo__GpuArch,
    .pExportInfo =        &__nvoc_export_info__GpuArch
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuArch __nvoc_metadata__GpuArch = {
    .rtti.pClassDef = &__nvoc_class_def_GpuArch,    // (gpuarch) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuArch,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(GpuArch, __nvoc_base_Object),
    .metadata__GpuHalspecOwner.rtti.pClassDef = &__nvoc_class_def_GpuHalspecOwner,    // (gpuhalspecowner) super
    .metadata__GpuHalspecOwner.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuHalspecOwner.rtti.offset    = NV_OFFSETOF(GpuArch, __nvoc_base_GpuHalspecOwner),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuArch = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__GpuArch.rtti,    // [0]: (gpuarch) this
        &__nvoc_metadata__GpuArch.metadata__Object.rtti,    // [1]: (obj) super
        &__nvoc_metadata__GpuArch.metadata__GpuHalspecOwner.rtti,    // [2]: (gpuhalspecowner) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuArch = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuArch object.
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_GpuHalspecOwner(GpuHalspecOwner*);
void __nvoc_dtor_GpuArch(GpuArch* pThis) {

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_GpuHalspecOwner(&pThis->__nvoc_base_GpuHalspecOwner);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_GpuArch(GpuArch *pThis) {
    ChipHal *chipHal = &staticCast(pThis, GpuHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // Hal field -- bGpuArchIsZeroFb
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->bGpuArchIsZeroFb = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bGpuArchIsZeroFb = ((NvBool)(0 != 0));
    }

    // Hal field -- bGpuarchSupportsIgpuRg
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->bGpuarchSupportsIgpuRg = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bGpuarchSupportsIgpuRg = ((NvBool)(0 != 0));
    }
}


// Construct GpuArch object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_GpuHalspecOwner(GpuHalspecOwner *);
NV_STATUS __nvoc_ctor_GpuArch(GpuArch *pGpuArch, NvU32 chipArch, NvU32 chipImpl, NvU32 hidrev, TEGRA_CHIP_TYPE tegraType) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pGpuArch->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GpuArch_fail_Object;
    status = __nvoc_ctor_GpuHalspecOwner(&pGpuArch->__nvoc_base_GpuHalspecOwner);
    if (status != NV_OK) goto __nvoc_ctor_GpuArch_fail_GpuHalspecOwner;

    // Initialize data fields.
    __nvoc_init_dataField_GpuArch(pGpuArch);

    // Call the constructor for this class.
    status = __nvoc_gpuarchConstruct(pGpuArch, chipArch, chipImpl, hidrev, tegraType);
    if (status != NV_OK) goto __nvoc_ctor_GpuArch_fail__init;
    goto __nvoc_ctor_GpuArch_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuArch_fail__init:
    __nvoc_dtor_GpuHalspecOwner(&pGpuArch->__nvoc_base_GpuHalspecOwner);
__nvoc_ctor_GpuArch_fail_GpuHalspecOwner:
    __nvoc_dtor_Object(&pGpuArch->__nvoc_base_Object);
__nvoc_ctor_GpuArch_fail_Object:
__nvoc_ctor_GpuArch_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuArch_1(GpuArch *pThis) {
    ChipHal *chipHal = &staticCast(pThis, GpuHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // gpuarchGetSystemPhysAddrWidth -- halified (4 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__gpuarchGetSystemPhysAddrWidth__ = &gpuarchGetSystemPhysAddrWidth_T234D;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__gpuarchGetSystemPhysAddrWidth__ = &gpuarchGetSystemPhysAddrWidth_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuarchGetSystemPhysAddrWidth__ = &gpuarchGetSystemPhysAddrWidth_TU102;
    }
    else
    {
        pThis->__gpuarchGetSystemPhysAddrWidth__ = &gpuarchGetSystemPhysAddrWidth_GH100;
    }

    // gpuarchGetDmaAddrWidth -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuarchGetDmaAddrWidth__ = &gpuarchGetDmaAddrWidth_GB10B;
    }
    // default
    else
    {
        pThis->__gpuarchGetDmaAddrWidth__ = &gpuarchGetDmaAddrWidth_b2b553;
    }

    // gpuarchGetGpcFuseStatusOffset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuarchGetGpcFuseStatusOffset__ = &gpuarchGetGpcFuseStatusOffset_GB10B;
    }
    // default
    else
    {
        pThis->__gpuarchGetGpcFuseStatusOffset__ = &gpuarchGetGpcFuseStatusOffset_b2b553;
    }
} // End __nvoc_init_funcTable_GpuArch_1 with approximately 8 basic block(s).


// Initialize vtable(s) for 3 virtual method(s).
void __nvoc_init_funcTable_GpuArch(GpuArch *pThis) {

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_GpuArch_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuArch(GpuArch *pThis,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_GpuHalspecOwner = &pThis->__nvoc_base_GpuHalspecOwner;    // (gpuhalspecowner) super
    pThis->__nvoc_pbase_GpuArch = pThis;    // (gpuarch) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);
    __nvoc_init__GpuHalspecOwner(&pThis->__nvoc_base_GpuHalspecOwner, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, TegraChipHal_tegraType);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuArch.metadata__Object;    // (obj) super
    pThis->__nvoc_base_GpuHalspecOwner.__nvoc_metadata_ptr = &__nvoc_metadata__GpuArch.metadata__GpuHalspecOwner;    // (gpuhalspecowner) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuArch;    // (gpuarch) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuArch(pThis);
}

NV_STATUS __nvoc_objCreate_GpuArch(GpuArch **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType, NvU32 chipArch, NvU32 chipImpl, NvU32 hidrev, TEGRA_CHIP_TYPE tegraType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuArch *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuArch));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuArch));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuArch(__nvoc_pThis, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, TegraChipHal_tegraType);
    __nvoc_status = __nvoc_ctor_GpuArch(__nvoc_pThis, chipArch, chipImpl, hidrev, tegraType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuArch_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuArch_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuArch));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuArch(GpuArch **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    NvU32 ChipHal_arch = va_arg(__nvoc_args, NvU32);
    NvU32 ChipHal_impl = va_arg(__nvoc_args, NvU32);
    NvU32 ChipHal_hidrev = va_arg(__nvoc_args, NvU32);
    TEGRA_CHIP_TYPE TegraChipHal_tegraType = va_arg(__nvoc_args, TEGRA_CHIP_TYPE);
    NvU32 chipArch = va_arg(__nvoc_args, NvU32);
    NvU32 chipImpl = va_arg(__nvoc_args, NvU32);
    NvU32 hidrev = va_arg(__nvoc_args, NvU32);
    TEGRA_CHIP_TYPE tegraType = va_arg(__nvoc_args, TEGRA_CHIP_TYPE);

    __nvoc_status = __nvoc_objCreate_GpuArch(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, TegraChipHal_tegraType, chipArch, chipImpl, hidrev, tegraType);

    return __nvoc_status;
}

