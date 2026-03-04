#define NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_ccsl_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x9bf1a1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Ccsl;

// Forward declarations for Ccsl
void __nvoc_init__Ccsl(Ccsl*);
void __nvoc_init_funcTable_Ccsl(Ccsl*);
NV_STATUS __nvoc_ctor_Ccsl(Ccsl*);
void __nvoc_init_dataField_Ccsl(Ccsl*);
void __nvoc_dtor_Ccsl(Ccsl*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Ccsl;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Ccsl;

// Down-thunk(s) to bridge Ccsl methods from ancestors (if any)

// Up-thunk(s) to bridge Ccsl methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_Ccsl = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Ccsl),
        /*classId=*/            classId(Ccsl),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Ccsl",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__Ccsl,
    /*pExportInfo=*/        &__nvoc_export_info__Ccsl
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__Ccsl __nvoc_metadata__Ccsl = {
    .rtti.pClassDef = &__nvoc_class_def_Ccsl,    // (ccsl) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Ccsl,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Ccsl = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__Ccsl.rtti,    // [0]: (ccsl) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Ccsl = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Ccsl(Ccsl *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Ccsl(Ccsl *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Ccsl(Ccsl *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_Ccsl(pThis);
    goto __nvoc_ctor_Ccsl_exit; // Success

__nvoc_ctor_Ccsl_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Ccsl_1(Ccsl *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Ccsl_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Ccsl(Ccsl *pThis) {
    __nvoc_init_funcTable_Ccsl_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Ccsl(Ccsl *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Ccsl = pThis;    // (ccsl) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Ccsl;    // (ccsl) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Ccsl(pThis);
}

