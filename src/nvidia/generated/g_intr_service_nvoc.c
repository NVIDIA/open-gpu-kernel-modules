#define NVOC_INTR_SERVICE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_intr_service_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2271cc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_funcTable_IntrService(IntrService*);
NV_STATUS __nvoc_ctor_IntrService(IntrService*);
void __nvoc_init_dataField_IntrService(IntrService*);
void __nvoc_dtor_IntrService(IntrService*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_IntrService;

static const struct NVOC_RTTI __nvoc_rtti_IntrService_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_IntrService,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_IntrService = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_IntrService_IntrService,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_IntrService,
    /*pExportInfo=*/        &__nvoc_export_info_IntrService
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_IntrService = 
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

    // intrservRegisterIntrService -- virtual
    pThis->__intrservRegisterIntrService__ = &intrservRegisterIntrService_IMPL;

    // intrservClearInterrupt -- virtual
    pThis->__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL;

    // intrservServiceInterrupt -- virtual
    pThis->__intrservServiceInterrupt__ = &intrservServiceInterrupt_IMPL;

    // intrservServiceNotificationInterrupt -- virtual
    pThis->__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL;
} // End __nvoc_init_funcTable_IntrService_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 4 virtual method(s).
void __nvoc_init_funcTable_IntrService(IntrService *pThis) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_IntrService_1(pThis);
}

void __nvoc_init_IntrService(IntrService *pThis) {
    pThis->__nvoc_pbase_IntrService = pThis;
    __nvoc_init_funcTable_IntrService(pThis);
}

