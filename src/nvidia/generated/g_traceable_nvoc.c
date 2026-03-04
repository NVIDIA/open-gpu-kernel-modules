#define NVOC_TRACEABLE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_traceable_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x6305d2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

// Forward declarations for OBJTRACEABLE
void __nvoc_init__OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_funcTable_OBJTRACEABLE(OBJTRACEABLE*);
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_dataField_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJTRACEABLE;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJTRACEABLE;

// Down-thunk(s) to bridge OBJTRACEABLE methods from ancestors (if any)

// Up-thunk(s) to bridge OBJTRACEABLE methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJTRACEABLE),
        /*classId=*/            classId(OBJTRACEABLE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJTRACEABLE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJTRACEABLE,
    /*pExportInfo=*/        &__nvoc_export_info__OBJTRACEABLE
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__OBJTRACEABLE __nvoc_metadata__OBJTRACEABLE = {
    .rtti.pClassDef = &__nvoc_class_def_OBJTRACEABLE,    // (traceable) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJTRACEABLE,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJTRACEABLE = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__OBJTRACEABLE.rtti,    // [0]: (traceable) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJTRACEABLE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_OBJTRACEABLE(pThis);
    goto __nvoc_ctor_OBJTRACEABLE_exit; // Success

__nvoc_ctor_OBJTRACEABLE_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJTRACEABLE_1(OBJTRACEABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJTRACEABLE_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    __nvoc_init_funcTable_OBJTRACEABLE_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJTRACEABLE(OBJTRACEABLE *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_OBJTRACEABLE = pThis;    // (traceable) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJTRACEABLE;    // (traceable) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJTRACEABLE(pThis);
}

