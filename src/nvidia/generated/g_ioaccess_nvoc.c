#define NVOC_IOACCESS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_ioaccess_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xfcaf2e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture;

// Forward declarations for RegisterAperture
void __nvoc_init__RegisterAperture(RegisterAperture*);
void __nvoc_init_funcTable_RegisterAperture(RegisterAperture*);
NV_STATUS __nvoc_ctor_RegisterAperture(RegisterAperture*);
void __nvoc_init_dataField_RegisterAperture(RegisterAperture*);
void __nvoc_dtor_RegisterAperture(RegisterAperture*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RegisterAperture;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RegisterAperture;

// Down-thunk(s) to bridge RegisterAperture methods from ancestors (if any)

// Up-thunk(s) to bridge RegisterAperture methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  regaprtReadReg08
//  regaprtReadReg16
//  regaprtReadReg32
//  regaprtWriteReg08
//  regaprtWriteReg16
//  regaprtWriteReg32
//  regaprtWriteReg32Uc
//  regaprtIsRegValid
const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RegisterAperture),
        /*classId=*/            classId(RegisterAperture),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RegisterAperture",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__RegisterAperture,
    /*pExportInfo=*/        &__nvoc_export_info__RegisterAperture
};


// Metadata with per-class RTTI and vtable
static const struct NVOC_METADATA__RegisterAperture __nvoc_metadata__RegisterAperture = {
    .rtti.pClassDef = &__nvoc_class_def_RegisterAperture,    // (regaprt) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RegisterAperture,
    .rtti.offset    = 0,

    .vtable.__regaprtReadReg08__ = NULL,    // pure virtual
    .vtable.__regaprtReadReg16__ = NULL,    // pure virtual
    .vtable.__regaprtReadReg32__ = NULL,    // pure virtual
    .vtable.__regaprtWriteReg08__ = NULL,    // pure virtual
    .vtable.__regaprtWriteReg16__ = NULL,    // pure virtual
    .vtable.__regaprtWriteReg32__ = NULL,    // pure virtual
    .vtable.__regaprtWriteReg32Uc__ = NULL,    // pure virtual
    .vtable.__regaprtIsRegValid__ = NULL,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RegisterAperture = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__RegisterAperture.rtti,    // [0]: (regaprt) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RegisterAperture = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RegisterAperture(RegisterAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RegisterAperture(RegisterAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RegisterAperture(RegisterAperture *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_RegisterAperture(pThis);
    goto __nvoc_ctor_RegisterAperture_exit; // Success

__nvoc_ctor_RegisterAperture_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RegisterAperture_1(RegisterAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RegisterAperture_1


// Initialize vtable(s) for 8 virtual method(s).
void __nvoc_init_funcTable_RegisterAperture(RegisterAperture *pThis) {
    __nvoc_init_funcTable_RegisterAperture_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RegisterAperture(RegisterAperture *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_RegisterAperture = pThis;    // (regaprt) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RegisterAperture;    // (regaprt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RegisterAperture(pThis);
}

