#define NVOC_HOST_ENG_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_host_eng_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xb356e7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHOSTENG;

// Forward declarations for OBJHOSTENG
void __nvoc_init__OBJHOSTENG(OBJHOSTENG*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_OBJHOSTENG(OBJHOSTENG*, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_OBJHOSTENG(OBJHOSTENG*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_OBJHOSTENG(OBJHOSTENG*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_OBJHOSTENG(OBJHOSTENG*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJHOSTENG;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJHOSTENG;

// Down-thunk(s) to bridge OBJHOSTENG methods from ancestors (if any)

// Up-thunk(s) to bridge OBJHOSTENG methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHOSTENG = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJHOSTENG),
        /*classId=*/            classId(OBJHOSTENG),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJHOSTENG",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJHOSTENG,
    /*pExportInfo=*/        &__nvoc_export_info__OBJHOSTENG
};


// Metadata with per-class RTTI and vtable
static const struct NVOC_METADATA__OBJHOSTENG __nvoc_metadata__OBJHOSTENG = {
    .rtti.pClassDef = &__nvoc_class_def_OBJHOSTENG,    // (hosteng) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJHOSTENG,
    .rtti.offset    = 0,

    .vtable.__hostengHaltAndReset__ = &hostengHaltAndReset_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJHOSTENG = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__OBJHOSTENG.rtti,    // [0]: (hosteng) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJHOSTENG = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJHOSTENG(OBJHOSTENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJHOSTENG(OBJHOSTENG *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_HOSTENG_ENSURE_HALT_SUCCEEDS_BEFORE_RESET
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_HOSTENG_ENSURE_HALT_SUCCEEDS_BEFORE_RESET, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_HOSTENG_ENSURE_HALT_SUCCEEDS_BEFORE_RESET, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJHOSTENG(OBJHOSTENG *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_OBJHOSTENG(pThis, pGpuhalspecowner);
    goto __nvoc_ctor_OBJHOSTENG_exit; // Success

__nvoc_ctor_OBJHOSTENG_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJHOSTENG_1(OBJHOSTENG *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
} // End __nvoc_init_funcTable_OBJHOSTENG_1


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_OBJHOSTENG(OBJHOSTENG *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    __nvoc_init_funcTable_OBJHOSTENG_1(pThis, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__OBJHOSTENG(OBJHOSTENG *pThis, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_OBJHOSTENG = pThis;    // (hosteng) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJHOSTENG;    // (hosteng) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJHOSTENG(pThis, pGpuhalspecowner);
}

