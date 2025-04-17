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
void __nvoc_init__OBJHOSTENG(OBJHOSTENG*);
void __nvoc_init_funcTable_OBJHOSTENG(OBJHOSTENG*);
NV_STATUS __nvoc_ctor_OBJHOSTENG(OBJHOSTENG*);
void __nvoc_init_dataField_OBJHOSTENG(OBJHOSTENG*);
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

void __nvoc_init_dataField_OBJHOSTENG(OBJHOSTENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJHOSTENG(OBJHOSTENG *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_OBJHOSTENG(pThis);
    goto __nvoc_ctor_OBJHOSTENG_exit; // Success

__nvoc_ctor_OBJHOSTENG_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJHOSTENG_1(OBJHOSTENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJHOSTENG_1


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_OBJHOSTENG(OBJHOSTENG *pThis) {
    __nvoc_init_funcTable_OBJHOSTENG_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJHOSTENG(OBJHOSTENG *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_OBJHOSTENG = pThis;    // (hosteng) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJHOSTENG;    // (hosteng) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJHOSTENG(pThis);
}

