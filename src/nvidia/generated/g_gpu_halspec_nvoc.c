#define NVOC_GPU_HALSPEC_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_halspec_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x34a6d6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner;

// Forward declarations for RmHalspecOwner
void __nvoc_init__RmHalspecOwner(RmHalspecOwner*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver);
void __nvoc_init_funcTable_RmHalspecOwner(RmHalspecOwner*);
NV_STATUS __nvoc_ctor_RmHalspecOwner(RmHalspecOwner*);
void __nvoc_init_dataField_RmHalspecOwner(RmHalspecOwner*);
void __nvoc_dtor_RmHalspecOwner(RmHalspecOwner*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RmHalspecOwner;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RmHalspecOwner;

// Down-thunk(s) to bridge RmHalspecOwner methods from ancestors (if any)

// Up-thunk(s) to bridge RmHalspecOwner methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmHalspecOwner),
        /*classId=*/            classId(RmHalspecOwner),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmHalspecOwner",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__RmHalspecOwner,
    /*pExportInfo=*/        &__nvoc_export_info__RmHalspecOwner
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__RmHalspecOwner __nvoc_metadata__RmHalspecOwner = {
    .rtti.pClassDef = &__nvoc_class_def_RmHalspecOwner,    // (rmhalspecowner) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmHalspecOwner,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RmHalspecOwner = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__RmHalspecOwner.rtti,    // [0]: (rmhalspecowner) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RmHalspecOwner = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmHalspecOwner(RmHalspecOwner *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmHalspecOwner(RmHalspecOwner *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmHalspecOwner(RmHalspecOwner *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_RmHalspecOwner(pThis);
    goto __nvoc_ctor_RmHalspecOwner_exit; // Success

__nvoc_ctor_RmHalspecOwner_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmHalspecOwner_1(RmHalspecOwner *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RmHalspecOwner_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RmHalspecOwner(RmHalspecOwner *pThis) {
    __nvoc_init_funcTable_RmHalspecOwner_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RmHalspecOwner(RmHalspecOwner *pThis,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_RmHalspecOwner = pThis;    // (rmhalspecowner) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RmHalspecOwner;    // (rmhalspecowner) this

    // Initialize halspec data.
    __nvoc_init_halspec_ChipHal(&pThis->chipHal, ChipHal_arch, ChipHal_impl, ChipHal_hidrev);
    __nvoc_init_halspec_RmVariantHal(&pThis->rmVariantHal, RmVariantHal_rmVariant);
    __nvoc_init_halspec_TegraChipHal(&pThis->tegraChipHal, TegraChipHal_tegraType);
    __nvoc_init_halspec_DispIpHal(&pThis->dispIpHal, DispIpHal_ipver);

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RmHalspecOwner(pThis);
}

