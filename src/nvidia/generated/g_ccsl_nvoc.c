#define NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_ccsl_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x9bf1a1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Ccsl;

void __nvoc_init_Ccsl(Ccsl*);
void __nvoc_init_funcTable_Ccsl(Ccsl*);
NV_STATUS __nvoc_ctor_Ccsl(Ccsl*);
void __nvoc_init_dataField_Ccsl(Ccsl*);
void __nvoc_dtor_Ccsl(Ccsl*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Ccsl;

static const struct NVOC_RTTI __nvoc_rtti_Ccsl_Ccsl = {
    /*pClassDef=*/          &__nvoc_class_def_Ccsl,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Ccsl,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Ccsl = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_Ccsl_Ccsl,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_Ccsl,
    /*pExportInfo=*/        &__nvoc_export_info_Ccsl
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_Ccsl = 
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

void __nvoc_init_Ccsl(Ccsl *pThis) {
    pThis->__nvoc_pbase_Ccsl = pThis;
    __nvoc_init_funcTable_Ccsl(pThis);
}

