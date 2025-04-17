#define NVOC_INTR_SERVICE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_intr_service_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x2271cc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

// Forward declarations for IntrService
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init_funcTable_IntrService(IntrService*);
NV_STATUS __nvoc_ctor_IntrService(IntrService*);
void __nvoc_init_dataField_IntrService(IntrService*);
void __nvoc_dtor_IntrService(IntrService*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__IntrService;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__IntrService;

// Down-thunk(s) to bridge IntrService methods from ancestors (if any)

// Up-thunk(s) to bridge IntrService methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(IntrService),
        /*classId=*/            classId(IntrService),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "IntrService",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__IntrService,
    /*pExportInfo=*/        &__nvoc_export_info__IntrService
};


// Metadata with per-class RTTI and vtable
static const struct NVOC_METADATA__IntrService __nvoc_metadata__IntrService = {
    .rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_IntrService,
    .rtti.offset    = 0,

    .vtable.__intrservRegisterIntrService__ = &intrservRegisterIntrService_IMPL,    // virtual
    .vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .vtable.__intrservServiceInterrupt__ = &intrservServiceInterrupt_IMPL,    // virtual
    .vtable.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__IntrService = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__IntrService.rtti,    // [0]: (intrserv) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__IntrService = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_IntrService(IntrService *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_IntrService(IntrService *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_IntrService(IntrService *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_IntrService(pThis);
    goto __nvoc_ctor_IntrService_exit; // Success

__nvoc_ctor_IntrService_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_IntrService_1(IntrService *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_IntrService_1


// Initialize vtable(s) for 4 virtual method(s).
void __nvoc_init_funcTable_IntrService(IntrService *pThis) {
    __nvoc_init_funcTable_IntrService_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__IntrService(IntrService *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_IntrService = pThis;    // (intrserv) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__IntrService;    // (intrserv) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_IntrService(pThis);
}

