#define NVOC_SPDM_PROXY_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_spdm_proxy_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x5b722d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SpdmProxy;

// Forward declarations for SpdmProxy
void __nvoc_init__SpdmProxy(SpdmProxy*);
void __nvoc_init_funcTable_SpdmProxy(SpdmProxy*);
NV_STATUS __nvoc_ctor_SpdmProxy(SpdmProxy*);
void __nvoc_init_dataField_SpdmProxy(SpdmProxy*);
void __nvoc_dtor_SpdmProxy(SpdmProxy*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SpdmProxy;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SpdmProxy;

// Down-thunk(s) to bridge SpdmProxy methods from ancestors (if any)

// Up-thunk(s) to bridge SpdmProxy methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_SpdmProxy = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SpdmProxy),
        /*classId=*/            classId(SpdmProxy),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SpdmProxy",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__SpdmProxy,
    /*pExportInfo=*/        &__nvoc_export_info__SpdmProxy
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__SpdmProxy __nvoc_metadata__SpdmProxy = {
    .rtti.pClassDef = &__nvoc_class_def_SpdmProxy,    // (spdmProxy) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SpdmProxy,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SpdmProxy = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__SpdmProxy.rtti,    // [0]: (spdmProxy) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SpdmProxy = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_SpdmProxy(SpdmProxy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SpdmProxy(SpdmProxy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_SpdmProxy(SpdmProxy *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_SpdmProxy(pThis);
    goto __nvoc_ctor_SpdmProxy_exit; // Success

__nvoc_ctor_SpdmProxy_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SpdmProxy_1(SpdmProxy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SpdmProxy_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SpdmProxy(SpdmProxy *pThis) {
    __nvoc_init_funcTable_SpdmProxy_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SpdmProxy(SpdmProxy *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_SpdmProxy = pThis;    // (spdmProxy) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SpdmProxy;    // (spdmProxy) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SpdmProxy(pThis);
}

