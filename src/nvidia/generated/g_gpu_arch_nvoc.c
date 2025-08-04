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
char __nvoc_class_id_uniqueness_check__0x4b33af = 1;
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
NV_STATUS __nvoc_ctor_GpuArch(GpuArch*, NvU32 arg_chipArch, NvU32 arg_chipImpl, NvU32 arg_hidrev, TEGRA_CHIP_TYPE arg_tegraType);
void __nvoc_init_dataField_GpuArch(GpuArch*);
void __nvoc_dtor_GpuArch(GpuArch*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuArch;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuArch;

// Down-thunk(s) to bridge GpuArch methods from ancestors (if any)

// Up-thunk(s) to bridge GpuArch methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuArch = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuArch),
        /*classId=*/            classId(GpuArch),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuArch",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuArch,
    /*pCastInfo=*/          &__nvoc_castinfo__GpuArch,
    /*pExportInfo=*/        &__nvoc_export_info__GpuArch
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_GpuHalspecOwner(GpuHalspecOwner*);
void __nvoc_dtor_GpuArch(GpuArch *pThis) {
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
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_GpuHalspecOwner(GpuHalspecOwner* );
NV_STATUS __nvoc_ctor_GpuArch(GpuArch *pThis, NvU32 arg_chipArch, NvU32 arg_chipImpl, NvU32 arg_hidrev, TEGRA_CHIP_TYPE arg_tegraType) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GpuArch_fail_Object;
    status = __nvoc_ctor_GpuHalspecOwner(&pThis->__nvoc_base_GpuHalspecOwner);
    if (status != NV_OK) goto __nvoc_ctor_GpuArch_fail_GpuHalspecOwner;
    __nvoc_init_dataField_GpuArch(pThis);

    status = __nvoc_gpuarchConstruct(pThis, arg_chipArch, arg_chipImpl, arg_hidrev, arg_tegraType);
    if (status != NV_OK) goto __nvoc_ctor_GpuArch_fail__init;
    goto __nvoc_ctor_GpuArch_exit; // Success

__nvoc_ctor_GpuArch_fail__init:
    __nvoc_dtor_GpuHalspecOwner(&pThis->__nvoc_base_GpuHalspecOwner);
__nvoc_ctor_GpuArch_fail_GpuHalspecOwner:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
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
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__gpuarchGetSystemPhysAddrWidth__ = &gpuarchGetSystemPhysAddrWidth_T234D;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuarchGetDmaAddrWidth__ = &gpuarchGetDmaAddrWidth_GB10B;
    }
    // default
    else
    {
        pThis->__gpuarchGetDmaAddrWidth__ = &gpuarchGetDmaAddrWidth_4a4dee;
    }

    // gpuarchIsZeroFb -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__gpuarchIsZeroFb__ = &gpuarchIsZeroFb_cbe027;
    }
    // default
    else
    {
        pThis->__gpuarchIsZeroFb__ = &gpuarchIsZeroFb_491d52;
    }

    // gpuarchSupportsIgpuRg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuarchSupportsIgpuRg__ = &gpuarchSupportsIgpuRg_cbe027;
    }
    // default
    else
    {
        pThis->__gpuarchSupportsIgpuRg__ = &gpuarchSupportsIgpuRg_491d52;
    }
} // End __nvoc_init_funcTable_GpuArch_1 with approximately 10 basic block(s).


// Initialize vtable(s) for 4 virtual method(s).
void __nvoc_init_funcTable_GpuArch(GpuArch *pThis) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_GpuArch(GpuArch **ppThis, Dynamic *pParent, NvU32 createFlags,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType, NvU32 arg_chipArch, NvU32 arg_chipImpl, NvU32 arg_hidrev, TEGRA_CHIP_TYPE arg_tegraType)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GpuArch *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GpuArch), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GpuArch));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__GpuArch(pThis, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, TegraChipHal_tegraType);
    status = __nvoc_ctor_GpuArch(pThis, arg_chipArch, arg_chipImpl, arg_hidrev, arg_tegraType);
    if (status != NV_OK) goto __nvoc_objCreate_GpuArch_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GpuArch_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GpuArch));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuArch(GpuArch **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    NvU32 ChipHal_arch = va_arg(args, NvU32);
    NvU32 ChipHal_impl = va_arg(args, NvU32);
    NvU32 ChipHal_hidrev = va_arg(args, NvU32);
    TEGRA_CHIP_TYPE TegraChipHal_tegraType = va_arg(args, TEGRA_CHIP_TYPE);
    NvU32 arg_chipArch = va_arg(args, NvU32);
    NvU32 arg_chipImpl = va_arg(args, NvU32);
    NvU32 arg_hidrev = va_arg(args, NvU32);
    TEGRA_CHIP_TYPE arg_tegraType = va_arg(args, TEGRA_CHIP_TYPE);

    status = __nvoc_objCreate_GpuArch(ppThis, pParent, createFlags, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, TegraChipHal_tegraType, arg_chipArch, arg_chipImpl, arg_hidrev, arg_tegraType);

    return status;
}

